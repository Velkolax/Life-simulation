//
// Created by tk2 on 11/18/25.
//

#include "Bacteria.h"
#include <iostream>
#include <random>
#include <algorithm>

int layers[] = {2, 3, 2};
int layerCount = sizeof(layers)/sizeof(layers[0]);

int GenerateRandomTrait(int range)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> disChange(0,range);
    return disChange(gen);
}

/*Bacteria::Bacteria()
{
    network = buildNetwork(layerCount, layers);
    initializeRandom(&network);
    lifespan = GenerateRandomTrait(10);
    maxEnergy = GenerateRandomTrait(250);
    energyLevel = maxEnergy;
    venomLevel = 0;
    upgradeLevel = 0;
    currentlifeTime = lifespan;
    std::fill_n(memory, MEMORY_SIZE, 0);
    this->Print();
    printf("Default Bacteria constructor end\n");
}*/

/*Bacteria::~Bacteria()
{
    printf("\033[35mBUMP\033[0m\n");
    getchar();
    freeNetwork(&network);
}*/

Bacteria::Bacteria(NeuralNetwork network, property lifespan, property energyLevel, property maxEnergy, property upgradeLevel, property venomLevel) : network(network),lifespan(lifespan), energy(energyLevel), maxEnergy(maxEnergy), protein(upgradeLevel), acid(venomLevel), currentlifeTime(lifespan)
{
    std::fill_n(memory, MEMORY_SIZE, 0);
}

Bacteria::Bacteria(Bacteria* bacteria1, Bacteria* bacteria2)
{
    Bacteria bac = bacteria1->Crossover(bacteria2);
    network = bac.network;
    lifespan = bac.lifespan;
    energy = bac.energy;
    maxEnergy = bac.maxEnergy;
    acid = bac.acid;
    currentlifeTime = lifespan;
    std::fill_n(memory, MEMORY_SIZE, 0);
}

Bacteria Bacteria::Crossover(Bacteria *bacteria2)
{
     auto bac = Bacteria(childNetwork(&network, &bacteria2->network, 0.2),
        (this->lifespan+bacteria2->lifespan)/2,
        (this->maxProtein+bacteria2->maxProtein)/2,
        (this->maxEnergy+bacteria2->maxEnergy)/2,
        (this->maxAcid+bacteria2->maxAcid)/2,
        (this->reflex+bacteria2->reflex)/2);
    return bac;
}

void MutateTrait(property& val, double mutationRate, int strength, property minVal, property maxVal) {

    static std::uniform_real_distribution<> disChance(0.0,1.0);

    if (disChance(gen) <= mutationRate) {
        std::uniform_int_distribution<> disChange(-strength,strength);
        val+= disChange(gen);
        val = std::max(minVal, std::min(val, maxVal));
    }
}



void Bacteria::Mutate() {
    double MUTATION_RATE = 0.01;

    // this->network = MutateNetwork(this->network);
    MutateTrait(maxEnergy, MUTATION_RATE, 50, 100, 250);
    MutateTrait(maxProtein, MUTATION_RATE, 1, 0, 10);
    MutateTrait(lifespan, MUTATION_RATE, 100, 50, 1000);
    MutateTrait(reflex, MUTATION_RATE, 10, 50, 100);
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
    std::cout << "LIFETIME: " << lifespan<< std::endl;
    std::cout << "CURRENT_LIFETIME" << currentlifeTime << std::endl;
    std::cout << "ENERGYLEVEL: " << energy<< std::endl;
    std::cout << "MAXENERGY: " << maxEnergy<< std::endl;
    std::cout << "UPGRADELEVEL: " << protein<< std::endl;
    std::cout << "VENOMLEVEL: " << acid<< std::endl;
    std::cout << "NETWORK:" << std::endl<< std::endl;
    printNetwork(&network);
    std::cout << "--------------------------------------------" << std::endl;
}

void Bacteria::defaultInitialization()
{
    network = buildNetwork(layerCount, layers);
    initializeRandom(&network);
    lifespan = GenerateRandomTrait(10);
    maxEnergy = GenerateRandomTrait(250);
    energy = maxEnergy;
    acid = 0;
    protein = 0;
    currentlifeTime = lifespan;
    std::fill_n(memory, MEMORY_SIZE, 0);
    this->Print();
}

void Bacteria::kill()
{
    freeNetwork(&network);
}
