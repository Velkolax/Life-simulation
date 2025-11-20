//
// Created by tk2 on 11/18/25.
//

#include "Bacteria.h"
#include <iostream>
#include <random>

int layers[] = {2, 3, 2};
int layerCount = sizeof(layers)/sizeof(layers[0]);

int GenerateRandomTrait(int range)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> disChange(0,range);
    return disChange(gen);
}

Bacteria::Bacteria()
{
    NeuralNetwork nn = buildNetwork(layerCount, layers);
    initializeRandom(&nn);
    network = nn;
    lifeTime = GenerateRandomTrait(10);
    maxEnergy = GenerateRandomTrait(500);
    energyLevel = maxEnergy;
    venomLevel = 0;
    upgradeLevel = 0;
    currentlifeTime = lifeTime;
    std::fill_n(memory, 12, 0);
    this->Print();
}

Bacteria::~Bacteria()
{
    // freeNetwork(&network);
}

Bacteria::Bacteria(NeuralNetwork network, int lifeTime, int energyLevel, int maxEnergy, int upgradeLevel, int venomLevel) : network(network),lifeTime(lifeTime), energyLevel(energyLevel), maxEnergy(maxEnergy), upgradeLevel(upgradeLevel), venomLevel(venomLevel), currentlifeTime(lifeTime)
{
    std::fill_n(memory, 12, 0);
}

Bacteria::Bacteria( Bacteria* bacteria1, Bacteria* bacteria2)
{
    Bacteria bac = bacteria1->Crossover(bacteria2);
    network = bac.network;
    lifeTime = bac.lifeTime;
    energyLevel = bac.energyLevel;
    maxEnergy = bac.maxEnergy;
    upgradeLevel = bac.upgradeLevel;
    venomLevel = bac.venomLevel;
    currentlifeTime = lifeTime;
    std::fill_n(memory, 12, 0);
}

Bacteria Bacteria::Crossover(Bacteria *bacteria2)
{
     auto bac = Bacteria(childNetwork(&network, &bacteria2->network, 0.2),
        (this->lifeTime+bacteria2->lifeTime)/2,
        (this->energyLevel+bacteria2->energyLevel)/2,
        (this->maxEnergy+bacteria2->maxEnergy)/2,
        (this->upgradeLevel+bacteria2->upgradeLevel)/2,
        (this->venomLevel+bacteria2->venomLevel)/2);
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
    double MUTATION_RATE = 0.1;

    // this->network = MutateNetwork(this->network);
    MutateTrait(maxEnergy, MUTATION_RATE, 50, 100, 5000);
    MutateTrait(upgradeLevel, MUTATION_RATE, 1, 0, 10);
    MutateTrait(lifeTime, MUTATION_RATE, 10, 50, 100);
}

void Bacteria::PassingOfTime(int dt) {
    currentlifeTime -= dt;
}

int Bacteria::GetCurrentLifeTime()
{
    return this->currentlifeTime;
}

bool Bacteria::CheckIfDead() {
    if (currentlifeTime<=0) {
        return true;
    }
    return false;
}

void Bacteria::Print()
{
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "BACTERIA" << std::endl;
    std::cout << "LIFETIME: " << lifeTime<< std::endl;
    std::cout << "CURRENT_LIFETIME" << currentlifeTime << std::endl;
    std::cout << "ENERGYLEVEL: " << energyLevel<< std::endl;
    std::cout << "MAXENERGY: " << maxEnergy<< std::endl;
    std::cout << "UPGRADELEVEL: " << upgradeLevel<< std::endl;
    std::cout << "VENOMLEVEL: " << venomLevel<< std::endl;
    std::cout << "NETWORK:" << std::endl<< std::endl;
    printNetwork(&network);
    std::cout << "--------------------------------------------" << std::endl;
}


