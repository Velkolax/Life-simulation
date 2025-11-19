//
// Created by tk2 on 11/18/25.
//

#include "Bacteria.h"

#include <random>


Bacteria Bacteria::Crossover(const Bacteria &bacteria2) const {
    const auto bac = Bacteria(CrossOver(this->network,bacteria2.network),
        (this->lifeTime+bacteria2.lifeTime)/2,
        (this->energyLevel+bacteria2.energyLevel)/2,
        (this->maxEnergy+bacteria2.maxEnergy)/2,
        (this->upgradeLevel+bacteria2.upgradeLevel)/2,
        (this->venomLevel+bacteria2.venomLevel/2));
    return bac;
}

void MutateTrait(int& val, double mutationRate, int strength, int minVal, int maxVal) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disChance(0.0,1.0);

    if (disChance(gen) <= mutationRate) {
        std::uniform_int_distribution<> disChange(-strength,strength);
        val+= disChange(gen);
        val = std::max(minVal, std::min(val, maxVal));
    }
}

void Bacteria::Mutate() {
    constexpr double MUTATION_RATE = 0.1;

    this->network = MutateNetwork(this->network);
    MutateTrait(maxEnergy, MUTATION_RATE, 50, 100, 5000);
    MutateTrait(venomLevel, MUTATION_RATE, 2, 0, 100);
    MutateTrait(upgradeLevel, MUTATION_RATE, 1, 0, 10);
    MutateTrait(lifeTime, MUTATION_RATE, 10, 50, 1000);
}

