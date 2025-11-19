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
    Bacteria(const NeuralNetwork &network, int lifeTime, int energyLevel, int maxEnergy, int upgradeLevel, int venomLevel) : network(network),lifeTime(lifeTime), energyLevel(energyLevel), maxEnergy(maxEnergy), upgradeLevel(upgradeLevel), venomLevel(venomLevel)
    {
        std::fill_n(memory, 12, 0);
    }
    Bacteria(const Bacteria& bacteria1, const Bacteria& bacteria2)
    {
        const Bacteria bac = bacteria1.Crossover(bacteria2);
        network = bac.network;
        lifeTime = bac.lifeTime;
        energyLevel = bac.energyLevel;
        maxEnergy = bac.maxEnergy;
        upgradeLevel = bac.upgradeLevel;
        venomLevel = bac.venomLevel;
        std::fill_n(memory, 12, 0);
    }
    [[nodiscard]] Bacteria Crossover(const Bacteria &bacteria2) const;
    void Mutate();


private:
    NeuralNetwork network{};
    char memory[12]{};
    Sight view[5][5];


    int lifeTime;
    int energyLevel;
    int maxEnergy;
    int upgradeLevel;
    int venomLevel;
};


#endif //LIFE_SIMULATION_BACTERIA_H