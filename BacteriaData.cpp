#include "BacteriaData.h"
#include <iostream>
#include "board.h"

/*BacteriaData::BacteriaData(glm::ivec2 pos,uint32_t id,uint32_t alive,int32_t lifespan):
    pos(pos),
    target_pos(pos),
    lifespan(lifespan),
    remaining_lifespan(lifespan),
    alive(alive)
{
    if (alive==1) fillNetworkWithRandom();
}*/

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

    age = 0;
}

/*
void BacteriaData::fillNetworkWithRandom()
{

        std::normal_distribution<float> distribution_W1(0.0f, std::sqrt(2.0f / INPUT_SIZE));
        std::normal_distribution<float> distribution_W2(0.0f, std::sqrt(2.0f / HIDDEN_SIZE));
        float* w1_start = (float*)&network[W1_OFFSET_VEC4];

        for (int i = 0; i < W1_SIZE_F; ++i) {
                w1_start[i] = distribution_W1(gen);
        }
        float* b1_start = (float*)&network[B1_OFFSET_VEC4];
        for (int i = 0; i < B1_SIZE_F; ++i) {
                b1_start[i] = 0.1f;
        }

        float* w2_start = (float*)&network[W2_OFFSET_VEC4];

        for (int i = 0; i < W2_SIZE_F; ++i) {
                w2_start[i] = distribution_W2(gen);
        }

        float* b2_start = (float*)&network[B2_OFFSET_VEC4];
        for (int i = 0; i < B2_SIZE_F; ++i) {
                b2_start[i] = 0.1f;
        }

}
*/


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
    auto& directions = (x & 1) oddDirections2l : evenDirections2l;
    auto& [dx, dy] = directions[clamp(int(dir * 6), 0, 5)];
    return board->getHexagon(x + dx, y + dy);
}

void BacteriaData::move(Board* board, float* data, coord x, coord y)
{
    int movesCount = speed / 20;
    if(speed > 4) speed = 4;
    Hexagon* oldHex = board->getHexagon(x, y);
    uint32_t id = oldHex->getData().bacteriaIndex;
    for(int i = 0; i < movesCount; i++)
    {
        if(!consumeEnergy(1f)) return;
        Hexagon* hex = directionToHex(board, data[i], oldHex->getX(), oldHex->getY());
        if(!empty(hex->getResident())) return; // próbuje wejść w coś
        hex->placeBacteria(board, id);
        oldHex->placeEmpty();
        oldHex = hex;
    }
}

void BacteriaData::attack(Board* board, float* data, coord x, coord y)
{
    Hexagon* hex = directionToHex(board, *data, x, y);
    if(!bacteria(hex->getResident()))
    {
        if(!consumeEnergy(2f)) return;
        return;
    }
    if(!consumeEnergy(4f)) return;
    BacteriaData& attacked = board->getBacteria(hex->getData().bacteriaIndex());
    int acidUsed = data[1] * acid;
    int sum = attacked.acid + attacked.energy + attacked.protein;
    int total = acidUsed * sum / MAX_STORED_VALUE;

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int r1 = dist(gen) * total;
    int r2 = dist(gen) * total;

    if (r1 > r2) std::swap(r1, r2);
    int acidDrained = r1;
    int energyDrained = r2 - r1;
    int proteinDrained = total - r2;

    attacked.acid -= acidDrained;
    attacked.energy -= energyDrained;
    attacked.protein -= proteinDrained;

    auto& directions = (x & 1) oddDirections2l : evenDirections2l;
    for(auto& [dx, dy] : directions)
    {
        if(!acidDrained && !energyDrained && !proteinDrained) break;
        Hexagon* h = board->getHexagon(hex->getX() + dx, hex->getY() + dy);
        if(::acid(h->getResident()))
        {
            int flow = (h->getData().acid.amount + acidDrained <= MAX_STORED_VALUE) ? acidDrained : MAX_STORED_VALUE - h->getData().acid.amount;
            h->getData().acid.amount += flow;
            acidDrained -= flow;
        }
        else if(::protein(h->getResident()))
        {
            int flow = (h->getData().protein.amount + proteinDrained <= MAX_STORED_VALUE) ? proteinDrained : MAX_STORED_VALUE - h->getData().protein.amount;
            h->getData().protein.amount += flow;
            proteinDrained -= flow;
        }
        else if(::energy(h->getResident()))
        {
            int flow = (h->getData().energy.amount + energyDrained <= MAX_STORED_VALUE) ? energyDrained : MAX_STORED_VALUE - h->getData().energy.amount;
            h->getData().energy.amount += flow;
            energyDrained -= flow;
        }
        else
        {
            if(acidDrained)
            {
                h->placeAcid(acidDrained);
                acidDrained = 0;
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

        board->acidShortage += acidDrained;
        board->proteinShortage += proteinDrained;
        // ilość energii nie musi pozostawać stała więc nie ma niedoboru
    }
}

void BacteriaData::breed(Board* board, float* data, coord x, coord y)
{
}

void BacteriaData::eat(Board* board, float* data, coord x, coord y)
{
    if(!consumeEnergy(2f)) return;
    Hexagon* hex = directionToHex(board, *data, x, y);
    if(!resource(hex->getResident())) return;
    int toEat = data[1] * hex->getData().acid.amount;
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
        int proteinPlacement = clamp(int(data[2] * 3), 0, 2);
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
    if(hex->getData().acid.amount == 0) hex->placeEmpty();
}

void BacteriaData::sleep(Board* board, float* data, coord x, coord y)
{
    if(!consumeEnergy(0.5f)) return;
}

static_assert(MEMORY_SIZE + 1 + 6 + TWO_NEIGHBOUR_LAYERS_SIZE * 3 <= SEND_SIZE, "SEND_SIZE too small");

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
