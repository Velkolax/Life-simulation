//
// Created by tk2 on 11/18/25.
//


#ifndef LIFE_SIMULATION_BACTERIA_H
#define LIFE_SIMULATION_BACTERIA_H

#define MEMORY_SIZE 16

#include <algorithm>

#include "NeuralNetwork.h"

typedef int property;

enum class SightType : unsigned char
{
    Stranger,
    Energy,
    Upgrade,
    Empty
};

class Sight
{
public:
    Sight() : type(SightType::Empty), energyCount(0), upgradeCount(0) {}
    Sight(SightType type, int energyCount, int upgradeCount): type(type), energyCount(energyCount), upgradeCount(upgradeCount){}
private:
    SightType type;
    int energyCount;
    int upgradeCount;
};



class Bacteria
{
public:
    Bacteria();
    ~Bacteria();
    Bacteria(NeuralNetwork network, property lifeTime, property energyLevel, property maxEnergy, property upgradeLevel, property venomLevel);
    Bacteria(Bacteria* bacteria1, Bacteria* bacteria2);

    Bacteria Crossover(Bacteria* bacteria2);
    void Mutate();
    void PassingOfTime(int dt);
    int GetCurrentLifeTime();
    bool CheckIfDead();
    void Print();

private:
    NeuralNetwork network{};
    char memory[MEMORY_SIZE]{};
    Sight view[5][5];


    property lifespan;
    property energyLevel;
    property maxEnergy;
    property upgradeLevel;
    property venomLevel;
    property currentlifeTime;
};


#endif //LIFE_SIMULATION_BACTERIA_H