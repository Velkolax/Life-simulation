#pragma once
#include <glm/glm.hpp>

constexpr int NETWORK_SIZE_VEC4 = 51;
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

    //alignas(16) glm::vec4 network[NETWORK_SIZE_VEC4];

    BacteriaData(
        glm::ivec2 pos,
        uint32_t id,
        uint32_t alive,
        int32_t lifespan)
        :
        pos(pos),
        target_pos(pos),
        id(id),
        lifespan(lifespan),
        remaining_lifespan(lifespan),
        alive(alive)
        {}
};



static_assert(sizeof(BacteriaData) % 16 == 0, "Struktura BacteriaData musi być wielokrotnością 16 bajtów!");