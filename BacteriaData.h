#pragma once

#include <random>
#include <array>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <glm/glm.hpp>


typedef int16_t coord;
typedef uint16_t ucoord;

class Hexagon;
class Board;
class Player;
class Country;
class Sight;
class Game;

inline std::mt19937 gen;
inline unsigned int Seed;

#define MEMORY_SIZE 3
#define MAX_ACCUSTOMABLE_VALUE 100
#define MAX_STORED_VALUE 200
#define SEND_SIZE 64
#define TWO_NEIGHBOUR_LAYERS_SIZE 18
#define BACTERIA_ACTIONS_NUMBER 5

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> evenDirections2l =
{{
    { 0, -1}, {-1, -1}, {-1,  0}, { 0,  1}, { 1,  0}, { 1, -1},
    { 0, -2}, {-1, -2}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  1},
    { 0,  2}, { 1,  1}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -2}
}};
static_assert(evenDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> oddDirections2l =
{{
    { 0, -1}, {-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}, { 1,  0},
    { 0, -2}, {-1, -1}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  2},
    { 0,  2}, { 1,  2}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -1}
}};
static_assert(oddDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);


struct BacteriaData
{
    float memory[MEMORY_SIZE];


    uint8_t lifespan;
    uint8_t speed;


    uint8_t acid;
    uint8_t energy;
    uint8_t protein;

    uint8_t age;

    void randomize();
    void addToBuffer(Board* board, float* buffer, coord x, coord y);


    inline bool consumeEnergy(float multiplier) { energy -= std::max(int(multiplier * (acid + protein + energy) * 0.05), 1); if(energy <= 0) { die(); return false; } return true; }
    void die();

    void move(Board* board, float* data, coord x, coord y);
    void attack(Board* board, float* data, coord x, coord y);
    void breed(Board* board, float* data, coord x, coord y);
    void eat(Board* board, float* data, coord x, coord y);
    void sleep(Board* board, float* data, coord x, coord y);

    inline static constexpr void (BacteriaData::*actions[BACTERIA_ACTIONS_NUMBER])(Board*, float*, coord, coord) =
    {
        &BacteriaData::move,
        &BacteriaData::attack,
        &BacteriaData::breed,
        &BacteriaData::eat,
        &BacteriaData::sleep
    };


    inline void execute(Board* board, float* data, coord x, coord y)
    {
        int index = std::clamp(int(*data * BACTERIA_ACTIONS_NUMBER), 0, BACTERIA_ACTIONS_NUMBER - 1);
        (this->*actions[index])(board, data + 1, x, y);
    }
};


static_assert(std::is_trivially_copyable_v<BacteriaData>);
static_assert(std::is_trivially_destructible_v<BacteriaData>);


struct AcidData
{
    uint8_t amount;
};
static_assert(std::is_trivially_constructible_v<AcidData>);
static_assert(std::is_trivially_copyable_v<AcidData>);


struct EnergyData
{
    uint8_t amount;
};
static_assert(std::is_trivially_constructible_v<EnergyData>);
static_assert(std::is_trivially_copyable_v<EnergyData>);


struct ProteinData
{
    uint8_t amount;
    float* sendToNetwork(Board* board, coord x, coord y);
};
static_assert(std::is_trivially_constructible_v<ProteinData>);
static_assert(std::is_trivially_copyable_v<ProteinData>);