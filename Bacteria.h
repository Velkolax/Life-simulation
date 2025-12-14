#pragma once
#include "NeuralNetwork.h"


typedef short property;
typedef short statistic;
class Board;
class Hexagon;

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

// static std::random_device rd;
// static std::mt19937 gen(rd());
#define MEMORY_SIZE 16

class Bacteria
{
public:
    Bacteria() = default;
    ~Bacteria() = default;
    void moveBacteria(int direction);
    void deleteBacteria();
    Bacteria(NeuralNetwork network, property lifeTime, property energyLevel, property maxEnergy, property upgradeLevel, property venomLevel);
    Bacteria(Bacteria* bacteria1, Bacteria* bacteria2);
    Bacteria Crossover(Bacteria* bacteria2);
    void Mutate();
    void PassingOfTime(int dt);
    int GetCurrentLifeTime();
    bool CheckIfDead();
    void Print();

    void act();

    void defaultInitialization(Hexagon *hex,Board *board);
    void kill();

private:
    NeuralNetwork network;
    char memory[MEMORY_SIZE];
    Sight view[5][5];
    Hexagon* hex;
    Board *board;
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
    property speed; // przyspiesza poruszanie ale zmniejsza częstotliwość ruchów
};