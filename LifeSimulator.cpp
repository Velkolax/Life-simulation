/*//
// Created by knotp on 19.11.2025.
//

#include "LifeSimulator.h"
#include <iostream>
#include <random>

void LifeSimulator::InitSimulation(int bacteriaCount)
{
    bacterias.reserve(bacteriaCount);
    for(int i = 0; i < bacteriaCount; i++)
    {
        //printf("\033[31mCREATION %d\033[0m\n", i);
        bacterias.emplace_back();
        bacterias.back().defaultInitialization();
        //printf("\033[31mCREATION END %d\033[0m\n", i);
    }
}

void LifeSimulator::Run()
{
    printf("\033[34mCHECKPOINT 1\033[0m\n");
    while (!bacterias.empty())
    {
        Step(1);
        getchar();
    }
}

void LifeSimulator::PrintAllRemainingTimes()
{
    std::cout << "LIFETIMES:" << std::endl;
    for (auto& b : bacterias)
    {
        std::cout << " " << b.GetCurrentLifeTime();
    }
    std::cout << std::endl;
}

template <typename T>
std::pair<T*, T*> pickTwoPointers(std::vector<T>& vec) {
    if (vec.size() < 2) {
        throw std::runtime_error("Za malo elementow");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> dist(0, vec.size()-1);

    size_t i1 = dist(gen);
    size_t i2 = dist(gen);
    return {&vec[i1], &vec[i2]};
}

void LifeSimulator::Step(int dt)
{
    std::cout << bacterias.size() << "\n";
    PrintAllRemainingTimes();

    auto [b1, b2] = pickTwoPointers(bacterias);
    bacterias.push_back(Bacteria(b1, b2));

    // std::cout << "PARENT1: ############################################" << std::endl;
    // b1->Print();
    // std::cout << "PARENT2: ############################################" << std::endl;
    // b2->Print();
    // std::cout << "CHILD: ############################################" << std::endl;
    // bac.Print();
    for (auto it = bacterias.begin(); it != bacterias.end(); )
    {
        
        it->Mutate();
        it->PassingOfTime(dt);
        if (it->CheckIfDead()) {
            it = bacterias.erase(it);
        }
        else {
            ++it;
        }
    }
}*/

