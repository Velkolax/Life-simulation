//
// Created by tk2 on 11/18/25.
//


#ifndef LIFE_SIMULATION_BACTERIA_H
#define LIFE_SIMULATION_BACTERIA_H

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
    Bacteria(NeuralNetwork network, int lifeTime, int energyLevel, int upgradeLevel, int venomLevel) : network(network),lifeTime(lifeTime), energyLevel(energyLevel), upgradeLevel(upgradeLevel), venomLevel(venomLevel)
    {
        for (auto& a : memory) { a = 0; }
    }

private:
    NeuralNetwork network;
    char memory[12];
    Sight view[5][5];


    int lifeTime;
    int energyLevel;
    int maxEnergy;
    int upgradeLevel;
    int venomLevel;
};


#endif //LIFE_SIMULATION_BACTERIA_H