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
    std::uniform_real_distribution<float> memoryDist(0., 100.);
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

void BacteriaData::sendToNetwork(coord x, coord y)
{
    float buffor[SEND_SIZE];
    float* f = buffor;

    memcpy(f, memory, sizeof(memory));
    f += MEMORY_SIZE;

    *f++ = float(x & 1); // 0 dla parzystego x, 1 dla nieparzystego x, przekazujemy to by ułatwić identyfikację okolicy bakterii

    *f++ = lifespan * INV_MAX_ACCUSTOMABLE;
    *f++ = speed * INV_MAX_ACCUSTOMABLE;
    *f++ = acid * INV_MAX_STORED;
    *f++ = energy * INV_MAX_STORED;
    *f++ = protein * INV_MAX_STORED;
    *f++ = age * INV_MAX_ACCUSTOMABLE;

    const auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;

    for (auto [dx, dy] : directions)
    {
        Hexagon* hex = board->getHexagon(x + dx, y + dy); // getHexagon() robi sprawdzanie zakresów

        Resident r = hex ? hex->getResident() : Resident::Wall;
        *f++ = float(r) * INV_RESIDENT;

        if (plain(r))
        {
            *f++ = 0.f;
            *f++ = 0.f;
        }
        else if (resource(r))
        {
            *f++ = hex->getData().acid.amount * INV_MAX_STORED;
            *f++ = 0.f;
        }
        else // bacteria
        {
            *f++ = hex->getData().bacteria.acid * INV_MAX_STORED; // przekazujemy ilość kwasu by oszacować czy jest groźna
            *f++ = hex->getData().bacteria.protein * INV_MAX_STORED; // przekazujemy ilość białka by oszacować czy jest bogata
        }
    }

    // Legendy głoszą że tu w swoim czasie pojawi się funkcja wysyłająca ten buffor
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
