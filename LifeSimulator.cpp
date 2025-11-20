//
// Created by knotp on 19.11.2025.
//

#include "LifeSimulator.h"
#include <iostream>
#include <random>

void LifeSimulator::InitSimulation(int bacteriaCount) {
    for (int i=0;i<bacteriaCount;i++) {
        bacteria.emplace_back();
    }
}

void LifeSimulator::Run() {
    while (!bacteria.empty()) {
        Step(1);
    }
}

template <typename T>
std::pair<const T*, const T*> pickTwoPointers(const std::vector<T>& vec) {
    if (vec.size() < 2) {
        throw std::runtime_error("Za malo elementow");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);

    size_t i1 = dist(gen);
    size_t i2 = dist(gen);

    while (i1 == i2) {
        i2 = dist(gen);
    }

    return {&vec[i1], &vec[i2]};
}

void LifeSimulator::Step(int dt) {

    std::cout << bacteria.size() << std::endl;
    auto [b1,b2] = pickTwoPointers(bacteria);
    bacteria.push_back(Bacteria(b1,b2));
    for (auto it = bacteria.begin(); it != bacteria.end(); ) {
        it->PassingOfTime(dt);
        if (it->CheckIfDead()) {
            it = bacteria.erase(it);
        }
        else {
            ++it;
        }
    }
}

