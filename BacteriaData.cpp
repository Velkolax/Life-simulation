#include "BacteriaData.h"
#include <iostream>
#include "board.h"
#include "game.h"
#include "simulation_engine.h"
#include "game_configdata.h"


void BacteriaData::randomize()
{
    std::uniform_real_distribution<float> memoryDist(0., 1.);
    for(int i = 0; i < MEMORY_SIZE; i++) memory[i] = memoryDist(gen);

    std::uniform_int_distribution<uint8_t> accustomableDist(1, MAX_ACCUSTOMABLE_VALUE/4);
    lifespan = accustomableDist(gen);
    speed = accustomableDist(gen);

    //std::uniform_int_distribution<uint8_t> storedDist(1, MAX_STORED_VALUE);
    acid = std::uniform_int_distribution<uint8_t>(0, MAX_STORED_VALUE/10)(gen);
    energy = std::uniform_int_distribution<uint8_t>(MAX_STORED_VALUE/4, MAX_STORED_VALUE)(gen);
    protein = std::uniform_int_distribution<uint8_t>(MAX_STORED_VALUE/10, MAX_STORED_VALUE/4)(gen);
    //protein = 0;
    age = 0;
}

constexpr float INV_RESIDENT = 1.0f / float(Resident::Protein);
constexpr float INV_MAX_ACCUSTOMABLE = 1.0f / MAX_ACCUSTOMABLE_VALUE;
constexpr float INV_MAX_STORED = 1.0f / MAX_STORED_VALUE;

void BacteriaData::addToBuffer(Board* board, float* buffer, coord x, coord y)
{
    memcpy(buffer, memory, sizeof(memory));
    buffer += MEMORY_SIZE;

    *buffer++ = float(x & 1); // 0 dla parzystego x, 1 dla nieparzystego x, przekazujemy to by ułatwić identyfikację okolicy bakterii

    *buffer++ = lifespan * INV_MAX_ACCUSTOMABLE;
    *buffer++ = speed * INV_MAX_ACCUSTOMABLE;
    *buffer++ = acid * INV_MAX_STORED;
    *buffer++ = energy * INV_MAX_STORED;
    *buffer++ = protein * INV_MAX_STORED;
    *buffer++ = age * INV_MAX_ACCUSTOMABLE;

    const auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;

    for (auto [dx, dy] : directions)
    {
        Hexagon* hex = board->getHexagon(x + dx, y + dy); // getHexagon() robi sprawdzanie zakresów

        Resident r = hex ? hex->getResident() : Resident::Wall;
        *buffer++ = float(r) * INV_RESIDENT;

        if (plain(r))
        {
            *buffer++ = 0.f;
            *buffer++ = 0.f;
        }
        else if (resource(r))
        {
            *buffer++ = hex->getData().acid.amount * INV_MAX_STORED;
            *buffer++ = 0.f;
        }
        else // bacteria
        {
            BacteriaData b = board->getBacteria(hex->getData().bacteriaIndex);
            *buffer++ = b.acid * INV_MAX_STORED; // przekazujemy ilość kwasu by oszacować czy jest groźna
            *buffer++ = b.protein * INV_MAX_STORED; // przekazujemy ilość białka by oszacować czy jest bogata
        }
    }
}

Hexagon* directionToHex(Board* board, float dir, coord x, coord y)
{
    auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;
    auto& [dx, dy] = directions[std::clamp(int(dir * 6), 0, 5)];
    return board->getHexagon(x + dx, y + dy);
}

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE+1> evenDirections2lws =
{{
    { 0,  0},
    { 0, -1}, {-1, -1}, {-1,  0}, { 0,  1}, { 1,  0}, { 1, -1},
    { 0, -2}, {-1, -2}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  1},
    { 0,  2}, { 1,  1}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -2}
}};
static_assert(evenDirections2lws.size() == TWO_NEIGHBOUR_LAYERS_SIZE + 1);

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE+1> oddDirections2lws =
{{
    { 0,  0},
    { 0, -1}, {-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}, { 1,  0},
    { 0, -2}, {-1, -1}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  2},
    { 0,  2}, { 1,  2}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -1}
}};
static_assert(oddDirections2lws.size() == TWO_NEIGHBOUR_LAYERS_SIZE + 1);

void BacteriaData::die(Board* board, coord x, coord y)
{
    Hexagon* hex = board->getHexagon(x, y);
    int32_t id = hex->getData().bacteriaIndex;
    int acidDrained = acid;
    int energyDrained = energy;
    int proteinDrained = protein + lifespan + speed + BACTERIA_BODY_SIZE;

    hex->placeEmpty();
    *this = BacteriaData{};
    board->addVacant(id);
    
    auto& directions = (x & 1) ? oddDirections2lws : evenDirections2lws;
    for(auto& [dx, dy] : directions)
    {
        if(!acidDrained && !energyDrained && !proteinDrained) break;
        Hexagon* h = board->getHexagon(hex->getX() + dx, hex->getY() + dy);
        if(!h) continue;
        if(::acid(h->getResident()))
        {
            int flow = std::min(acidDrained, MAX_STORED_VALUE - h->getData().acid.amount);
            h->getData().acid.amount += flow;
            acidDrained -= flow;
        }
        else if(::protein(h->getResident()))
        {
            int flow = std::min(proteinDrained, MAX_STORED_VALUE - h->getData().protein.amount);
            h->getData().protein.amount += flow;
            proteinDrained -= flow;
        }
        else if(::energy(h->getResident()))
        {
            int flow = std::min(energyDrained, MAX_STORED_VALUE - h->getData().energy.amount);
            h->getData().energy.amount += flow;
            energyDrained -= flow;
        }
        else if(empty(h->getResident()))
        {
            if(proteinDrained)
            {
                int flow = std::min(proteinDrained, MAX_STORED_VALUE);
                h->placeProtein(flow);
                proteinDrained -= flow;
            }
            else if(acidDrained)
            {
                int flow = std::min(acidDrained, MAX_STORED_VALUE);
                h->placeAcid(flow);
                acidDrained -= flow;
            }
            else if(energyDrained)
            {
                int flow = std::min(energyDrained, MAX_STORED_VALUE);
                h->placeEnergy(flow);
                energyDrained -= flow;
            }
            else break;
        }
    }
    board->acidShortage += acidDrained;
    board->proteinShortage += proteinDrained;
}


void BacteriaData::execute(Board* board, float* data, coord x, coord y)
{
    //std::cout << "WYJŚCIE 1: " << data[0] << std::endl;
    //int index = std::clamp(int(*data * BACTERIA_ACTIONS_NUMBER), 0, BACTERIA_ACTIONS_NUMBER - 1);
    int pos = std::clamp(int(*data * 7), 0, 6);
    if(pos)
    {
        auto& directions = (x & 1) ? oddDirections2lws : evenDirections2lws;
        auto& [dx, dy] = directions[pos];
        Hexagon* hex = board->getHexagon(x + dx, y + dy);
        if (hex!=nullptr)
        {
            (this->*interactionsInEnumOrder[(int)hex->getResident()])(board, hex, data + 1, x, y);
        }

    }
    else
    {
        sleep(board, NULL, data + 1, x, y);
    }

    //(this->*actions[index])(board, data + 1, x, y);
}


/*void BacteriaData::move(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::MoveFailure;
    int movesCount = std::min(speed / 25, 3) + 1;
    Hexagon* oldHex = board->getHexagon(x, y);
    int32_t id = oldHex->getData().bacteriaIndex;

    for(int i = 0; i < movesCount; i++)
    {
        if(!consumeEnergy(GameConfigData::getFloat("moveCost"), board, oldHex->getX(), oldHex->getY())) return;
        Hexagon* hex = directionToHex(board, data[i], oldHex->getX(), oldHex->getY());
        if(!hex || !empty(hex->getResident())) return;
        hex->placeBacteria(board, id);
        oldHex->placeEmpty();
        oldHex = hex;
        lastAction = Action::Move;
    }

}*/

void BacteriaData::move(Board* board, Hexagon* hex, float* data, coord x, coord y)
{
    lastAction = Action::MoveFailure;
    int reps = repeats(*data);
    Hexagon* oldHex = board->getHexagon(x, y);
    int32_t id = oldHex->getData().bacteriaIndex;

    for(int i = 0; i < reps; i++)
    {
        if(!hex || !empty(hex->getResident())) return;
        if(!consumeEnergy(GameConfigData::getFloat("moveCost"), board, oldHex->getX(), oldHex->getY())) return;
        hex->placeBacteria(board, id);
        oldHex->placeEmpty();
        oldHex = hex;
        hex = directionToHex(board, data[i], oldHex->getX(), oldHex->getY());
        lastAction = Action::Move;
    }
}


/*void BacteriaData::attack(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::AttackFailure;
    Hexagon* hex = directionToHex(board, *data, x, y);
    if(!hex || !bacteria(hex->getResident()))
    {
        if(!consumeEnergy(GameConfigData::getFloat("attackCost"), board, x, y)) return;
        return;
    }
    if(!consumeEnergy(GameConfigData::getFloat("attackCostNext"), board, x, y)) return;
    BacteriaData& attacked = board->getBacteria(hex->getData().bacteriaIndex);
    int acidUsed = std::clamp(int(data[1] * acid), 0, (int)acid);
    acid -= acidUsed;
    int sum = attacked.acid + attacked.energy + attacked.protein;
    int total = acidUsed * sum / MAX_STORED_VALUE;

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int r1 = dist(gen) * total;
    int r2 = dist(gen) * total;

    if (r1 > r2) std::swap(r1, r2);
    int acidDrained = r1;
    int energyDrained = r2 - r1;
    int proteinDrained = total - r2;

    acidDrained = std::min(acidDrained, (int)attacked.acid);
    energyDrained = std::min(energyDrained, (int)attacked.energy);
    proteinDrained = std::min(proteinDrained, (int)attacked.protein);

    attacked.acid -= acidDrained;
    attacked.energy -= energyDrained;
    attacked.protein -= proteinDrained;

    if(attacked.energy == 0) attacked.die(board, hex->getX(), hex->getY());

    acidDrained += acidUsed;

    auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;
    for(auto& [dx, dy] : directions)
    {
        if(!acidDrained && !energyDrained && !proteinDrained) break;
        Hexagon* h = board->getHexagon(hex->getX() + dx, hex->getY() + dy);
        if(!h) continue;
        if(::acid(h->getResident()))
        {
            int flow = std::min(acidDrained, MAX_STORED_VALUE - h->getData().acid.amount);
            h->getData().acid.amount += flow;
            acidDrained -= flow;
        }
        else if(::protein(h->getResident()))
        {
            int flow = std::min(proteinDrained, MAX_STORED_VALUE - h->getData().protein.amount);
            h->getData().protein.amount += flow;
            proteinDrained -= flow;
        }
        else if(::energy(h->getResident()))
        {
            int flow = std::min(energyDrained, MAX_STORED_VALUE - h->getData().energy.amount);
            h->getData().energy.amount += flow;
            energyDrained -= flow;
        }
        else if(empty(h->getResident()))
        {
            if(acidDrained)
            {
                int flow = std::min(acidDrained, MAX_STORED_VALUE); // Połączenie wysączonego i użytego kwasu, jako jedyny ma szansę być więszky niż MAX_STORED_VALUE
                h->placeAcid(flow);
                acidDrained -= flow;
            }
            else if(proteinDrained)
            {
                h->placeProtein(proteinDrained);
                proteinDrained = 0;
            }
            else if(energyDrained)
            {
                h->placeEnergy(energyDrained);
                energyDrained = 0;
            }
            else break;
        }
    }

    board->acidShortage += acidDrained;
    board->proteinShortage += proteinDrained;
    // ilość energii nie musi pozostawać stała więc nie ma niedoboru

    lastAction = Action::Attack;
}*/

void BacteriaData::attack(Board* board, Hexagon* attackedHex, float* data, coord x, coord y)
{
    lastAction = Action::AttackFailure;
    int reps = repeats(*data);

    for (int i = 0; i < reps; i++)
    {
        if(!consumeEnergy(GameConfigData::getFloat("attackCostNext"), board, x, y)) return;
        if(!attackedHex || !bacteria(attackedHex->getResident())) return;
        int acidUsed = std::clamp(int(*data * acid), 0, (int)acid);
        if(!acidUsed) return;
        BacteriaData& attacked = board->getBacteria(attackedHex->getData().bacteriaIndex);
        acid -= acidUsed;
        int sum = attacked.acid + attacked.energy + attacked.protein;
        int total = acidUsed * sum / MAX_STORED_VALUE;

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        int r1 = dist(gen) * total;
        int r2 = dist(gen) * total;

        if (r1 > r2) std::swap(r1, r2);
        int acidDrained = r1;
        int energyDrained = r2 - r1;
        int proteinDrained = total - r2;

        acidDrained = std::min(acidDrained, (int)attacked.acid);
        energyDrained = std::min(energyDrained, (int)attacked.energy);
        proteinDrained = std::min(proteinDrained, (int)attacked.protein);

        attacked.acid -= acidDrained;
        attacked.energy -= energyDrained;
        attacked.protein -= proteinDrained;

        if(attacked.energy == 0) attacked.die(board, attackedHex->getX(), attackedHex->getY());

        acidDrained += acidUsed;

        auto& directions = (attackedHex->getX() & 1) ? oddDirections2l : evenDirections2l;
        for(auto& [dx, dy] : directions)
        {
            if(!acidDrained && !energyDrained && !proteinDrained) break;
            Hexagon* h = board->getHexagon(attackedHex->getX() + dx, attackedHex->getY() + dy);
            if(!h) continue;
            if(::acid(h->getResident()))
            {
                int flow = std::min(acidDrained, MAX_STORED_VALUE - h->getData().acid.amount);
                h->getData().acid.amount += flow;
                acidDrained -= flow;
            }
            else if(::protein(h->getResident()))
            {
                int flow = std::min(proteinDrained, MAX_STORED_VALUE - h->getData().protein.amount);
                h->getData().protein.amount += flow;
                proteinDrained -= flow;
            }
            else if(::energy(h->getResident()))
            {
                int flow = std::min(energyDrained, MAX_STORED_VALUE - h->getData().energy.amount);
                h->getData().energy.amount += flow;
                energyDrained -= flow;
            }
            else if(empty(h->getResident()))
            {
                if(acidDrained)
                {
                    int flow = std::min(acidDrained, MAX_STORED_VALUE); // Połączenie wysączonego i użytego kwasu, jako jedyny ma szansę być więszky niż MAX_STORED_VALUE
                    h->placeAcid(flow);
                    acidDrained -= flow;
                }
                else if(proteinDrained)
                {
                    h->placeProtein(proteinDrained);
                    proteinDrained = 0;
                }
                else if(energyDrained)
                {
                    h->placeEnergy(energyDrained);
                    energyDrained = 0;
                }
                else break;
            }
        }

        board->acidShortage += acidDrained;
        board->proteinShortage += proteinDrained;
        // ilość energii nie musi pozostawać stała więc nie ma niedoboru

        lastAction = Action::Attack;
    }
}


void BacteriaData::cross(BacteriaData& mom,int energySent,int lifespanSent,int speedSent)
{
    lifespan = lifespanSent;
    speed = speedSent;
    acid = 0;
    energy=energySent;
    protein=0;
    age=0;
}

/*void BacteriaData::breed(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::BreedFailure;
    Hexagon* oldHex = board->getHexagon(x, y);
    Hexagon *hex = directionToHex(board,*data,x,y);
    if (!this->consumeEnergy(GameConfigData::getFloat("breedCost"),board, x,y)) return;
    if (this->protein < BACTERIA_BODY_SIZE + 1 + 1) return;
    this->protein -= (BACTERIA_BODY_SIZE + 1 + 1);

    int energySent = data[1] * this->energy;
    if (this->energy<energySent) return;

    int lifespanSent = data[2] * this->protein;
    if (lifespanSent>=MAX_ACCUSTOMABLE_VALUE) lifespanSent=MAX_ACCUSTOMABLE_VALUE-1;
    int speedSent = (1-data[2]) * this->protein;
    if (speedSent>=MAX_ACCUSTOMABLE_VALUE) speedSent=MAX_ACCUSTOMABLE_VALUE-1;

    if (!hex || !bacteria(hex->getResident())) return;

    float lifespanIncrease = data[1];

    std::vector<std::pair<coord,coord>> possibleDirections;
    auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;
    for(auto& [dx,dy] : directions)
    {
        Hexagon* h = board->getHexagon(hex->getX() + dx, hex->getY() + dy);
        if (!h) continue;
        if (::empty(h->getResident()))
        {
            possibleDirections.push_back({dx,dy});
        }
    }
    if (possibleDirections.empty()) return;
    std::uniform_int_distribution<int> dist(0,possibleDirections.size()-1);
    int index = dist(gen);
    std::pair<coord,coord> dir = possibleDirections[index];
    Hexagon* childHex = board->getHexagon(hex->getX()+dir.first,hex->getY()+dir.second);
    if (!childHex || !empty(childHex->getResident())) return;

    childHex->placeChild(board,*this,energySent,lifespanSent,speedSent);

    if (!board->emptyVacant())
    {
        board->getGame()->engine->reproduceNetwork(oldHex->getData().bacteriaIndex,hex->getData().bacteriaIndex, board->popVacant());
    }

    this->lastAction = Action::Breed;
}*/

void BacteriaData::breed(Board* board, Hexagon* dadHex, float* data, coord x, coord y)
{
    lastAction = Action::BreedFailure;
    if (!this->consumeEnergy(GameConfigData::getFloat("breedCost"), board, x, y)) return;
    if (!dadHex || !bacteria(dadHex->getResident())) return;

    Hexagon* momHex = board->getHexagon(x, y);

    int reps = repeats(*data);

    for (int i = 0; i < reps; i++)
    {
        int energySent = data[1] * this->energy;
        if (this->energy <= energySent)
        {
            if(lastAction == Action::BreedFailure) lastAction = Action::BreedFailureNoEnergy;
            return;
        }

        int lifespanSent = std::min(int(data[2] * this->protein * 0.3), MAX_ACCUSTOMABLE_VALUE);
        if (!lifespanSent) lifespanSent = 1;

        int speedSent = std::min(int(data[3] * this->protein * 0.3), MAX_ACCUSTOMABLE_VALUE);
        if (!speedSent) speedSent = 1;

        if (BACTERIA_BODY_SIZE + lifespanSent + speedSent > this->protein)
        {
            if(BACTERIA_BODY_SIZE + 2 > this->protein)
            {
                if(lastAction == Action::BreedFailure) lastAction = Action::BreedFailureNoProtein;
                return;
            }
            int tempProtein = this->protein;
            tempProtein -= BACTERIA_BODY_SIZE;
            float sum = data[2] + data[3];
            if (sum <= 0.0f)
            {
                lifespanSent = 1;
                speedSent = 1;
            }
            else
            {
                lifespanSent = tempProtein * data[2] / sum;
                lifespanSent = std::max(1, lifespanSent);
                if(lifespanSent == tempProtein) lifespanSent--;
                speedSent = tempProtein - lifespanSent;
                std::cout << "Emergency resource split: " << lifespanSent << " / " << speedSent << " (" << (int)this->protein << ")\n";
            }
        }

        std::vector<Hexagon*> possiblePlacements;
        possiblePlacements.reserve(8);
        auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;
        for(auto& [dx,dy] : directions)
        {
            Hexagon* h = board->getHexagon(x + dx, y + dy);
            if (h && (empty(h->getResident()) || resource(h->getResident())))
            {
                possiblePlacements.push_back(h);
            }
        }
        if (possiblePlacements.empty())
        {
            if(lastAction == Action::BreedFailure) lastAction = Action::BreedFailureNoSpace;
            return;
        }

        this->protein -= BACTERIA_BODY_SIZE + lifespanSent + speedSent;
        this->energy -= energySent;

        Hexagon* childHex = possiblePlacements[std::uniform_int_distribution<int>(0, possiblePlacements.size() - 1)(gen)];

        if(::acid(childHex->getResident())) board->acidShortage += childHex->getData().acid.amount;
        if(::protein(childHex->getResident())) board->proteinShortage += childHex->getData().protein.amount;

        childHex->placeChild(board, *this, energySent, lifespanSent, speedSent);

        if (!board->emptyVacant())
        {
            board->getGame()->engine->reproduceNetwork(momHex->getData().bacteriaIndex, dadHex->getData().bacteriaIndex, board->popVacant());
        }

        this->lastAction = Action::Breed;
    }
}


/*void BacteriaData::eat(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::EatFailure;
    if(!consumeEnergy(GameConfigData::getFloat("eatCost"), board, x, y)) return;
    Hexagon* hex = directionToHex(board, *data, x, y);
    if(!hex || !resource(hex->getResident())) return;
    int toEat = data[1] * hex->getData().acid.amount; // wszystkie zasoby mają tylko parametr amount więc pobranie go z byle którego nic nie zmienia
    if(::acid(hex->getResident()))
    {
        if(toEat > MAX_STORED_VALUE - acid) toEat = MAX_STORED_VALUE - acid;
        acid += toEat;
    }
    else if(::energy(hex->getResident()))
    {
        if(toEat > MAX_STORED_VALUE - energy) toEat = MAX_STORED_VALUE - energy;
        energy += toEat;
    }
    else // białko
    {
        int proteinPlacement = std::clamp(int(data[2] * 3), 0, 2);
        if(proteinPlacement == 0) // magazyn
        {
            if(toEat > MAX_STORED_VALUE - protein) toEat = MAX_STORED_VALUE - protein;
            protein += toEat;
        }
        else if(proteinPlacement == 1) // żywotność
        {
            if(toEat > MAX_ACCUSTOMABLE_VALUE - lifespan) toEat = MAX_ACCUSTOMABLE_VALUE - lifespan;
            lifespan += toEat;
        }
        else // prędkość
        {
            if(toEat > MAX_ACCUSTOMABLE_VALUE - speed) toEat = MAX_ACCUSTOMABLE_VALUE - speed;
            speed += toEat;
        }
    }
    hex->getData().acid.amount -= toEat;
    if(hex->getData().acid.amount == 0) hex->placeEmpty();
    lastAction = Action::Eat;
}*/

void BacteriaData::eat(Board* board, Hexagon* eatenHex, float* data, coord x, coord y)
{
    lastAction = Action::EatFailure;
    int reps = repeats(*data);
    for(int i = 0; i < reps; i++)
    {
        if(!consumeEnergy(GameConfigData::getFloat("eatCost"), board, x, y)) return;
        if(!eatenHex || !resource(eatenHex->getResident())) return;
        int toEat = std::max(int(data[1] * eatenHex->getData().acid.amount), 1); // wszystkie zasoby mają tylko parametr amount więc pobranie go z byle którego nic nie zmienia
        if(::acid(eatenHex->getResident()))
        {
            toEat = std::min(toEat, MAX_STORED_VALUE - acid);
            acid += toEat;
        }
        else if(::energy(eatenHex->getResident()))
        {
            toEat = std::min(toEat, MAX_STORED_VALUE - energy);
            energy += toEat;
        }
        else // białko
        {
            int proteinPlacement = std::clamp(int(data[2] * 3), 0, 2);
            if(proteinPlacement == 0) // magazyn
            {
                toEat = std::min(toEat, MAX_STORED_VALUE - protein);
                protein += toEat;
            }
            else if(proteinPlacement == 1) // żywotność
            {
                toEat = std::min(toEat, MAX_ACCUSTOMABLE_VALUE - lifespan);
                lifespan += toEat;
            }
            else // prędkość
            {
                toEat = std::min(toEat, MAX_ACCUSTOMABLE_VALUE - speed);
                speed += toEat;
            }
        }
        eatenHex->getData().acid.amount -= toEat;
        if(eatenHex->getData().acid.amount == 0) eatenHex->placeEmpty();
        lastAction = Action::Eat;
    }
}


/*void BacteriaData::sleep(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::SleepFailure;
    if(!consumeEnergy(GameConfigData::getFloat("sleepCost"), board, x, y)) return;
    lastAction = Action::Sleep;
}*/


float sleepEnergyUseMultiplier[] = { 0.5f, 0.2f, 0.05f, 0.01f };

void BacteriaData::sleep(Board* board, Hexagon* hex, float* data, coord x, coord y)
{
    lastAction = Action::SleepFailure;
    int reps = repeats(*data);
    if(!consumeEnergy(GameConfigData::getFloat("sleepCost") * sleepEnergyUseMultiplier[reps-1], board, x, y)) return;
    lastAction = Action::Sleep;
}

static_assert(MEMORY_SIZE + 1 + 6 + TWO_NEIGHBOUR_LAYERS_SIZE * 3 == INPUT, "SEND_SIZE too small");


