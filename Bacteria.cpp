//
// Created by tk2 on 11/18/25.
//

#include "Bacteria.h"
#include <iostream>
#include <random>

int GenerateRandomTrait(int range) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> disChange(0,range);
    return disChange(gen);
}

Bacteria::Bacteria() {
    int layers[] = {2,3,2};
    NeuralNetwork nn = buildNetwork(std::size(layers),layers);
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

//TODO: Add networks to crossover. I got random memory leaks from this
Bacteria Bacteria::Crossover(Bacteria *bacteria2)  {
    srand((unsigned int)time(NULL));
    int layers[] = {2, 3, 2};
    int layerCount = sizeof(layers)/sizeof(layers[0]);
    NeuralNetwork nn1 = buildNetwork(layerCount, layers);
    initializeRandom(&nn1);
    printNetwork(&nn1);
    printf("\n");
    NeuralNetwork nn2 = buildNetwork(layerCount, layers);
    initializeRandom(&nn2);
    printNetwork(&nn2);
    printf("\n");
    NeuralNetwork net = childNetwork(&network,&nn2,0.2);
     auto bac = Bacteria(this->network,
        (this->lifeTime+bacteria2->lifeTime)/2,
        (this->energyLevel+bacteria2->energyLevel)/2,
        (this->maxEnergy+bacteria2->maxEnergy)/2,
        (this->upgradeLevel+bacteria2->upgradeLevel)/2,
        (this->venomLevel+bacteria2->venomLevel/2));
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
    MutateTrait(lifeTime, MUTATION_RATE, 1, 5, 10);
}

void Bacteria::PassingOfTime(int dt) {
    currentlifeTime -= dt;
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
    std::cout << "ENERGYLEVEL: " << energyLevel<< std::endl;
    std::cout << "MAXENERGY: " << maxEnergy<< std::endl;
    std::cout << "UPGRADELEVEL: " << upgradeLevel<< std::endl;
    std::cout << "VENOMLEVEL: " << venomLevel<< std::endl;
    std::cout << "NETWORK:" << std::endl<< std::endl;
    printNetwork(&network);
    std::cout << "--------------------------------------------" << std::endl;
}


