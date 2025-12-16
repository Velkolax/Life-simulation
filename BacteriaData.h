#pragma once
#include <random>
#include <glm/glm.hpp>
#include <random>

inline std::mt19937 gen;


#define MEMORY_SIZE 4

#define MAX_ACCUSTOMABLE_VALUE 100
#define MAX_STORED_VALUE 200


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