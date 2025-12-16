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
