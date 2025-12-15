#pragma once
#include <random>
#include <glm/glm.hpp>

constexpr int INPUT_SIZE = 6;
constexpr int HIDDEN_SIZE = 16;
constexpr int OUTPUT_SIZE = 7;

constexpr int W1_SIZE = INPUT_SIZE * HIDDEN_SIZE;
constexpr int B1_SIZE = HIDDEN_SIZE;
constexpr int W2_SIZE = HIDDEN_SIZE * OUTPUT_SIZE;
constexpr int B2_SIZE = OUTPUT_SIZE;

constexpr int TOTAL_PARAMS = W1_SIZE + B1_SIZE + W2_SIZE + B2_SIZE;

constexpr int NETWORK_SIZE_VEC4 = (TOTAL_PARAMS + 3) / 4;

constexpr int W1_SIZE_F = INPUT_SIZE * HIDDEN_SIZE;
constexpr int B1_SIZE_F = HIDDEN_SIZE;
constexpr int W2_SIZE_F = HIDDEN_SIZE * OUTPUT_SIZE;
constexpr int B2_SIZE_F = OUTPUT_SIZE;

constexpr int W1_OFFSET_VEC4 = 0;
constexpr int W1_SIZE_VEC4 = (W1_SIZE_F + 3) / 4;

constexpr int B1_OFFSET_VEC4 = W1_OFFSET_VEC4 + W1_SIZE_VEC4;
constexpr int B1_SIZE_VEC4 = (B1_SIZE_F + 3) / 4;

constexpr int W2_OFFSET_VEC4 = B1_OFFSET_VEC4 + B1_SIZE_VEC4;
constexpr int W2_SIZE_VEC4 = (W2_SIZE_F + 3) / 4;

constexpr int B2_OFFSET_VEC4 = W2_OFFSET_VEC4 + W2_SIZE_VEC4;
constexpr int B2_SIZE_VEC4 = (B2_SIZE_F + 3) / 4;
// Just a small data class for transport purposes
// Now only implementing basic stuff
struct alignas(16) BacteriaData
{

    glm::ivec2 pos;
    glm::ivec2 target_pos;

    uint32_t id;
    int32_t lifespan;
    int32_t remaining_lifespan;
    uint32_t alive;

    glm::vec4 network[NETWORK_SIZE_VEC4];

    BacteriaData(glm::ivec2 pos,uint32_t id,uint32_t alive,int32_t lifespan);
    void fillNetworkWithRandom();
};



static_assert(sizeof(BacteriaData) % 16 == 0, "Struktura BacteriaData musi być wielokrotnością 16 bajtów!");