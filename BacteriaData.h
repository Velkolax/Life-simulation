#pragma once
#include <random>
#include <glm/glm.hpp>
#include <random>


typedef int16_t coord;
typedef uint16_t ucoord;

inline std::mt19937 gen;


#define MEMORY_SIZE 3

#define MAX_ACCUSTOMABLE_VALUE 100
#define MAX_STORED_VALUE 200

#define SEND_SIZE 64
#define TWO_NEIGHBOUR_LAYERS_SIZE 18 // 6 sąsiadów w pierwszej warstwie i 12 w drugiej


constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> evenDirections2l =
{
    // Pierwsza warstwa
    { 0, -1}, // górny
    {-1, -1}, // lewy górny
    {-1,  0}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  0}, // prawy dolny
    { 1, -1}, // prawy górny

    // Druga warstwa
    { 0, -2},
    {-1, -2},
    {-2, -1},
    {-2,  0},
    {-2,  1},
    {-1,  1},
    { 0,  2},
    { 1,  1},
    { 2,  1},
    { 2,  0},
    { 2, -1},
    { 1, -2}
};
static_assert(evenDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);

constexpr std::array<std::pair<coord, coord>, TWO_NEIGHBOUR_LAYERS_SIZE> oddDirections2l =
{
    // Pierwsza warstwa
    { 0, -1}, // górny
    {-1,  0}, // lewy górny
    {-1,  1}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  1}, // prawy dolny
    { 1,  0}, // prawy górny

    // Druga warstwa
    { 0, -2},
    {-1, -1},
    {-2, -1},
    {-2,  0},
    {-2,  1},
    {-1,  2},
    { 0,  2},
    { 1,  2},
    { 2,  1},
    { 2,  0},
    { 2, -1},
    { 1, -1}
};
static_assert(oddDirections2l.size() == TWO_NEIGHBOUR_LAYERS_SIZE);


struct BacteriaData
{
    float memory[MEMORY_SIZE];

    // Zarządzalne
    uint8_t lifespan;
    uint8_t speed;

    // Przechowywane
    uint8_t acid;
    uint8_t energy;
    uint8_t protein;

    // Cechy
    uint8_t age;

    void randomize();
    void sendToNetwork(Board* board, coord x, coord y);
};

static_assert(std::is_trivially_constructible_v<BacteriaData>);
static_assert(std::is_trivially_copyable_v<BacteriaData>);
static_assert(std::is_trivially_destructible_v<BacteriaData>);


struct AcidData
{
    uint8_t amount;
};

static_assert(std::is_trivially_constructible_v<AcidData>);
static_assert(std::is_trivially_copyable_v<AcidData>);
static_assert(std::is_trivially_destructible_v<AcidData>);


struct EnergyData
{
    uint8_t amount;
};

static_assert(std::is_trivially_constructible_v<EnergyData>);
static_assert(std::is_trivially_copyable_v<EnergyData>);
static_assert(std::is_trivially_destructible_v<EnergyData>);


struct ProteinData
{
    uint8_t amount;
};

static_assert(std::is_trivially_constructible_v<ProteinData>);
static_assert(std::is_trivially_copyable_v<ProteinData>);
static_assert(std::is_trivially_destructible_v<ProteinData>);

//static_assert(sizeof(BacteriaData) % 16 == 0, "Struktura BacteriaData musi być wielokrotnością 16 bajtów!");