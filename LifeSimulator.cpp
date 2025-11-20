//
// Created by knotp on 19.11.2025.
//

#include "LifeSimulator.h"
#include <iostream>
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

void LifeSimulator::Step(int dt) {

    std::cout << bacteria.size() << std::endl;
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

