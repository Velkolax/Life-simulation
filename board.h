#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <random>

#include "NeuralNetwork.h"
#include "glm/vec2.hpp"
#include "BacteriaData.h"


typedef uint16_t clan_t;

enum class Resident : clan_t
{
    // Plain
    Wall,
    Empty,

    // Resource
    Acid,
    Energy,
    Protein,

    // Bacteria
    Bacteria,
};

inline bool plain(Resident resident) noexcept { return resident == Resident::Wall || resident == Resident::Empty; };
inline bool wall(Resident resident) noexcept { return resident == Resident::Wall; };
inline bool empty(Resident resident) noexcept { return resident == Resident::Empty; };
inline bool resource(Resident resident) noexcept { return resident >= Resident::Acid && resident <= Resident::Protein; };
inline bool acid(Resident resident) noexcept { return resident == Resident::Acid; };
inline bool energy(Resident resident) noexcept { return resident == Resident::Energy; };
inline bool protein(Resident resident) noexcept { return resident == Resident::Protein; };
inline bool bacteria(Resident resident) noexcept { return resident >= Resident::Bacteria; };
inline bool clanned(Resident resident) noexcept { return resident > Resident::Bacteria; };


union ResidentData
{
    int32_t bacteriaIndex; // indeks w tablicy bacterias
    //BacteriaData bacteria;
    EnergyData energy;
    ProteinData protein;
    AcidData acid;
};

class Game;
class SimulationEngine;

class Hexagon
{
private:
    const coord x;
    const coord y;
    Resident resident; // enum o wymuszonym rozmiarze bajta
    ResidentData data;

public:
    // Ilość kwasu i białka na planszy ma pozostawać stała więc jeśli jakiś zniknie (np ze względu na brak miejsca) musi zostać dodany przy następnym zrzucie zasobów
    int acidShortage = 0;
    int proteinShortage = 0;

    Hexagon();
    Hexagon(coord x, coord y);
    Hexagon(coord x, coord y, Resident resident);

    inline coord getX() const noexcept { return x; }
    inline coord getY() const noexcept { return y; }
    inline glm::ivec2 getPos() const noexcept { return glm::ivec2(x, y); }
    inline Resident getResident() const noexcept { return resident; }
    inline clan_t getClan() const noexcept { if(clanned(resident)) return clan_t(int(resident) - int(Resident::Bacteria)); else return 0; };
    //inline void setResident(Resident resident) noexcept { this->resident = resident; } // Używaj funkcji place zamiast tego
    inline ResidentData& getData() noexcept { return data; }

    void placeWall();
    void placeEmpty();
    void placeAcid();
    void placeAcid(uint8_t amount);
    void placeEnergy();
    void placeEnergy(uint8_t amount);
    void placeProtein();
    void placeProtein(int number);
    void placeProtein(uint8_t amount);
    void placeBacteriaC(Board* board, clan_t clan);
    void placeBacteriaCB(Board* board, Resident clannedBacteria);
    void placeChild(Board* board, BacteriaData& mom, Resident clannedBacteria, int energySent, int lifespanSent, int speedSent);
    void importBacteria(Resident clannedBacteria, uint32_t id);

    //bool isNearWater(Board* board);

    std::vector<Hexagon*> neighbours(Board* board, int recursion = 0, bool includeSelf = false, std::function<bool(Hexagon*)> filter = nullptr);
    std::vector<Hexagon*> doubleFilterNeighbours(Board* board, int recursion, bool includeSelf, std::function<bool(Hexagon*)> expansionFilter, std::function<bool(Hexagon*)> resultFilter);
};

class Board
{
private:
    const coord width;
    const coord height;
    std::vector<Hexagon> board;
    std::vector<BacteriaData> bacterias;
    std::vector<int32_t> vacantSlots;

    uint64_t step = 0;

    Game* game;

public:
    int acidShortage = 0;
    int proteinShortage = 0;
    int highestAge=0;
    // inicjalizatory
    Board(coord width, coord height, Game* game, int bacteriaCount);
    void InitializeRandom(int min, int max);
    void InitializeNeighbour(int recursion, bool includeMiddle);

    void tick();
    void resourcesMerge();
    size_t getProteinCount();
    int getHighestAge();
    int getLowestAge();
    double getAvgEnergy();
    float getFailureRatio();
    float getActionPercentage(Action a);
    void spawnFood(double foodRatio);
    void spawnBacteria(int bacteriaCount, clan_t clansCount);
    void spawnProteinFromShortage();
    bool isResourceOverLimit();
    void pushResourcesToCenter();
    // gettery/settery
    inline coord getWidth() const noexcept { return width; }
    inline coord getHeight() const noexcept { return height; }
    inline Hexagon* getHexagon(coord x, coord y) { return (x < 0 || y < 0 || x >= width || y >= height) ? nullptr : &(board[y * width + x]); }
    inline Hexagon* getHexagon(int i) { return (i < 0 || i >= width * height) ? nullptr : &(board[i]); }
    inline int getBacteriaCount() const noexcept { return bacterias.size(); }
    int getAliveBacteriaCount();
    inline BacteriaData& getBacteria(int i) noexcept { return bacterias[i]; }
    inline int32_t addBacteria() noexcept
    {
        if (!vacantSlots.empty()) return popVacant();
        bacterias.emplace_back();
        return (int32_t)bacterias.size() -1;
    }
    inline void addVacant(int32_t i) noexcept { vacantSlots.push_back(i); }
    inline int32_t popVacant() noexcept { int32_t r = vacantSlots.back(); vacantSlots.pop_back(); return r; }
    inline bool emptyVacant() noexcept { return vacantSlots.empty(); }
    inline uint64_t getStep() const noexcept { return step; }
    inline const Game* getGame() const noexcept { return game; }

};

