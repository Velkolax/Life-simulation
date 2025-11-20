//
// Created by tk2 on 11/18/25.
//


#ifndef LIFE_SIMULATION_BACTERIA_H
#define LIFE_SIMULATION_BACTERIA_H

#include <algorithm>

#include "NeuralNetwork.h"

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
    Bacteria(NeuralNetwork network, int lifeTime, int energyLevel, int maxEnergy, int upgradeLevel, int venomLevel);
    Bacteria( Bacteria* bacteria1, Bacteria* bacteria2);

    Bacteria Crossover(Bacteria* bacteria2);
    void Mutate();
    void PassingOfTime(int dt);
    int GetCurrentLifeTime();
    bool CheckIfDead();
    void Print();

private:
    NeuralNetwork network{};
    char memory[12]{};
    Sight view[5][5];


    int lifeTime;
    int energyLevel;
    int maxEnergy;
    int upgradeLevel;
    int venomLevel;
    int currentlifeTime;
};


#endif //LIFE_SIMULATION_BACTERIA_H