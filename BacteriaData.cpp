#include "BacteriaData.h"
#include <iostream>
#include "board.h"
#include "game.h"
#include "simulation_engine.h"


void BacteriaData::randomize()
{
    std::uniform_real_distribution<float> memoryDist(0., 1.);
    for(int i = 0; i < MEMORY_SIZE; i++) memory[i] = memoryDist(gen);

    std::uniform_int_distribution<uint8_t> accustomableDist(1, MAX_ACCUSTOMABLE_VALUE);
    lifespan = accustomableDist(gen);
    speed = accustomableDist(gen);

    std::uniform_int_distribution<uint8_t> storedDist(1, MAX_STORED_VALUE);
    acid = storedDist(gen);
    energy = storedDist(gen);
    protein = storedDist(gen);
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
    hex->placeEmpty();
    *this = BacteriaData{};
    board->addVacant(id);

    int acidDrained = acid;
    int energyDrained = energy;
    int proteinDrained = protein + lifespan + speed + BACTERIA_BODY_SIZE;
    
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

void BacteriaData::move(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::MoveFailure;
    int movesCount = std::min(speed / 20, 4);
    Hexagon* oldHex = board->getHexagon(x, y);
    int32_t id = oldHex->getData().bacteriaIndex;
    for(int i = 0; i < movesCount; i++)
    {
        if(!consumeEnergy(0.1f, board, oldHex->getX(), oldHex->getY())) return;
        Hexagon* hex = directionToHex(board, data[i], oldHex->getX(), oldHex->getY());
        // if (!hex) {std::cout << "BRAK HEXA!" << std::endl; return;}
        // if (!empty(hex->getResident())) {std::cout << "HEX NIE JEST PUSTY" << std::endl; return;}
        if(!hex || !empty(hex->getResident())) return;
        hex->placeBacteria(board, id);
        oldHex->placeEmpty();
        oldHex = hex;
        //std::cout << "I MOVED!!!" << std::endl;
        lastAction = Action::Move;
    }

}

void BacteriaData::attack(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::AttackFailure;
    Hexagon* hex = directionToHex(board, *data, x, y);
    if(!hex || !bacteria(hex->getResident()))
    {
        if(!consumeEnergy(2.f, board, x, y)) return;
        return;
    }
    if(!consumeEnergy(4.f, board, x, y)) return;
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
}

void BacteriaData::cross(BacteriaData& mom)
{
    lifespan = mom.lifespan;
    speed = mom.speed;
    acid = 10;
    energy=5;
    protein=5;
    age=0;
}

void BacteriaData::breed(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::BreedFailure;
    Hexagon* oldHex = board->getHexagon(x, y);
    Hexagon *hex = directionToHex(board,*data,x,y);
    if (!hex || !bacteria(hex->getResident())) return;
    if (this->protein < 5) return;
    if (!this->consumeEnergyValue(5,board, x,y)) return;
    this->protein -= 5;


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

    childHex->placeChild(board,*this);

    if (!board->emptyVacant())
    {
        board->getGame()->engine->reproduceNetwork(oldHex->getData().bacteriaIndex,hex->getData().bacteriaIndex, board->popVacant());
    }

    this->lastAction = Action::Breed;
}

void BacteriaData::eat(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::EatFailure;
    //if(!consumeEnergy(02.f, board, x, y)) return;
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
}

void BacteriaData::sleep(Board* board, float* data, coord x, coord y)
{
    lastAction = Action::SleepFailure;
    if(!consumeEnergy(0.5f, board, x, y)) return;
    lastAction = Action::Sleep;
}

static_assert(MEMORY_SIZE + 1 + 6 + TWO_NEIGHBOUR_LAYERS_SIZE * 3 == INPUT, "SEND_SIZE too small");

/*AcidData::AcidData(uint8_t amount)
{
    this->amount = amount;
}

EnergyData::EnergyData(uint8_t amount)
{
    this->amount = amount;
}

ProteinData::ProteinData(uint8_t amount)
{
    this->amount = amount;
}*/
