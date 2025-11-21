//
// Created by tk2 on 11/18/25.
//


#ifndef LIFE_SIMULATION_BACTERIA_H
#define LIFE_SIMULATION_BACTERIA_H

#define MEMORY_SIZE 16

#include <algorithm>

#include "NeuralNetwork.h"

typedef short property;
typedef short statistic;

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
    Bacteria() = default;
    ~Bacteria() = default;
    Bacteria(NeuralNetwork network, property lifeTime, property energyLevel, property maxEnergy, property upgradeLevel, property venomLevel);
    Bacteria(Bacteria* bacteria1, Bacteria* bacteria2);

    Bacteria Crossover(Bacteria* bacteria2);
    void Mutate();
    void PassingOfTime(int dt);
    int GetCurrentLifeTime();
    bool CheckIfDead();
    void Print();

    void defaultInitialization();
    void kill();

private:
    NeuralNetwork network;
    char memory[MEMORY_SIZE];
    Sight view[5][5];

    // statystyki to zasoby i wiedza o bakterii
    statistic currentlifeTime;
    statistic protein;
    statistic energy;
    statistic acid;

    // property mogą być podnoszone przez białka
    property lifespan; // przedłuża życie ale zwiększa podatność na choroby (nagła śmierć)
    property maxProtein; 
    property maxEnergy;
    property maxAcid;
    property reflex; // zwiększa częstotliwość ruchów ale spowalnia poruszanie
};


#endif //LIFE_SIMULATION_BACTERIA_H