#pragma once

#include <random>
#include <array>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <iostream>
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

#define MEMORY_SIZE 3
#define MAX_ACCUSTOMABLE_VALUE 100
#define MAX_STORED_VALUE 200
#define TWO_NEIGHBOUR_LAYERS_SIZE 18
//#define BACTERIA_ACTIONS_NUMBER 5

#define BACTERIA_BODY_SIZE 10

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> evenDirections2l =
{{
    { 0, -1}, {-1, -1}, {-1,  0}, { 0,  1}, { 1,  0}, { 1, -1},
    { 0, -2}, {-1, -2}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  1}, { 0,  2}, { 1,  1}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -2}
}};
static_assert(evenDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> oddDirections2l =
{{
    { 0, -1}, {-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}, { 1,  0},
    { 0, -2}, {-1, -1}, {-2, -1}, {-2,  0}, {-2,  1}, {-1,  2}, { 0,  2}, { 1,  2}, { 2,  1}, { 2,  0}, { 2, -1}, { 1, -1}
}};
static_assert(oddDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);

static inline std::string stringActions[] =
{
    "Nothing",
    "Attack", "Sleep", "Eat", "Breed", "Move",
    "AttackFailure", "SleepFailure", "EatFailure", "BreedFailure", "MoveFailure",
    "BreedFailureNoEnergy", "BreedFailureNoProtein", "BreedFailureNoSpace"
};

enum class Action
{
    Nothing,
    Attack, Sleep, Eat, Breed, Move,
    AttackFailure, SleepFailure, EatFailure, BreedFailure, MoveFailure,
    BreedFailureNoEnergy, BreedFailureNoProtein, BreedFailureNoSpace
};

struct BacteriaData
{
    float memory[MEMORY_SIZE];


    uint8_t lifespan;
    uint8_t speed;


    uint8_t acid;
    uint8_t energy;
    uint8_t protein;

    uint8_t age;

    uint8_t kills = 0;
    uint8_t mothered = 0;
    uint8_t fathered = 0;
    Action lastAction = Action::Nothing;

    void randomize();
    void cross(BacteriaData& mom,int energySent,int lifespanSent,int speedSent);
    void addToBuffer(Board* board, float* buffer, coord x, coord y);

    inline void printBacteria()
    {
        std::cout << "Baceria:\n- age: " << (int)age << "\n- last action: " << stringActions[(int)lastAction] << "\n- kills: " << (int)kills << "\n- mothered: " << (int)mothered << ", fathered: " << (int)fathered << '\n'; 
    }


    inline bool consumeEnergy(float multiplier, Board* board, coord x, coord y)
    {
        int e = int(energy) - std::max(int(multiplier * (acid + protein + energy) * 0.05), 1);
        if (e <= 0) { die(board, x, y); return false; }
        energy = e;
        return true;
    }

    inline bool consumeEnergyValue(int value, Board* board, coord x, coord y)
    {
        int e = int(energy) - value;
        if (e <= 0) { die(board, x, y); return false; }
        energy = e;
        return true;
    }
    
    inline void getOlder(Board* board, coord x, coord y)
    {
        age += 1;
        if(age > lifespan) die(board, x, y);
        float ageNorm = float(age) / float(lifespan);
        float lifespanNorm = float(lifespan) / 100.0f;
        float diseaseChance = 0.0001f + ageNorm * ageNorm * lifespanNorm * 0.02f;

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(gen) < diseaseChance) die(board, x, y); // Choroba (prawdopodobieństwo wzrasta z lifespan)
    }

    inline int repeats(float f)
    {
        return std::clamp(std::min(int(f * 4), speed / 25), 0, 3) + 1;
    }

    void die(Board* board, coord x, coord y);

    void move(Board* board, Hexagon* hex, float* data, coord x, coord y);
    void attack(Board* board, Hexagon* attackedHex, float* data, coord x, coord y);
    void breed(Board* board, Hexagon* dadHex, float* data, coord x, coord y);
    void eat(Board* board, Hexagon* eatenHex, float* data, coord x, coord y);
    void sleep(Board* board, Hexagon* hex, float* data, coord x, coord y);

    /*inline static constexpr void (BacteriaData::*actions[BACTERIA_ACTIONS_NUMBER])(Board*, float*, coord, coord) =
    {
        &BacteriaData::attack,
        &BacteriaData::sleep,
        &BacteriaData::eat,
        &BacteriaData::breed,
        &BacteriaData::move,
    };*/

    inline static constexpr void (BacteriaData::*interactionsInEnumOrder[])(Board*, Hexagon*, float*, coord, coord) =
    {
        &BacteriaData::sleep,   // Wall
        &BacteriaData::move,    // Empty
        &BacteriaData::breed,   // Bacteria
        &BacteriaData::eat,     // Acid
        &BacteriaData::eat,     // Energy
        &BacteriaData::eat      // Protein
    };

    void execute(Board* board, float* data, coord x, coord y);
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
};
static_assert(std::is_trivially_constructible_v<ProteinData>);
static_assert(std::is_trivially_copyable_v<ProteinData>);