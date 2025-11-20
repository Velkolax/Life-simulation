//
// Created by knotp on 19.11.2025.
//

#ifndef LIFE_SIMULATION_LIFESIMULATOR_H
#define LIFE_SIMULATION_LIFESIMULATOR_H
#include <vector>

#include "Bacteria.h"


class LifeSimulator
{
private:
    std::vector<Bacteria> bacterias;
public:
    explicit LifeSimulator(int bacteriaCount){InitSimulation(bacteriaCount);}
    void InitSimulation(int bacteriaCount);
    void Run();
    void Step(int dt);
};


#endif //LIFE_SIMULATION_LIFESIMULATOR_H