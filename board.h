#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <random>
#include <unistd.h>
#include "NeuralNetwork.h"
#include "glm/vec2.hpp"
#include "BacteriaData.h"

static int counter = 0;
#define BIG_NUMBER 10000000

enum class Resident : uint8_t
{
    // Plain
    Wall,
    Empty,

    // Bacteria
    Bacteria,

    // Resource
    Acid,
    Energy,
    Protein
};

inline bool plain(Resident resident) noexcept { return resident == Resident::Wall || resident == Resident::Empty; };
inline bool wall(Resident resident) noexcept { return resident == Resident::Wall; };
inline bool empty(Resident resident) noexcept { return resident == Resident::Empty; };
inline bool bacteria(Resident resident) noexcept { return resident == Resident::Bacteria; };
inline bool resource(Resident resident) noexcept { return resident >= Resident::Acid && resident <= Resident::Protein; };
inline bool acid(Resident resident) noexcept {return resident == Resident::Acid;};
inline bool energy(Resident resident) noexcept { return resident == Resident::Energy; };
inline bool protein(Resident resident) noexcept { return resident == Resident::Protein; };


union ResidentData
{
    uint32_t bacteriaIndex; // indeks w tablicy bacterias
    //BacteriaData bacteria;
    EnergyData energy;
    ProteinData protein;
    AcidData acid;
};



class Hexagon
{
private:
    const coord x;
    const coord y;
    Resident resident; // enum o wymuszonym rozmiarze bajta
    ResidentData data;
public:
    Hexagon();
    Hexagon(coord x, coord y);
    Hexagon(coord x, coord y, Resident resident);

    inline coord getX() const noexcept { return x; }
    inline coord getY() const noexcept { return y; }
    inline glm::ivec2 getPos() const noexcept { return glm::ivec2(x, y); }
    inline Resident getResident() const noexcept { return resident; }
    //inline void setResident(Resident resident) noexcept { this->resident = resident; } // Używaj funkcji place zamiast tego
    inline const ResidentData& getData() const noexcept { return data; }

    void placeWall();
    void placeEmpty();
    void placeAcid();
    void placeAcid(uint8_t amount);
    void placeEnergy();
    void placeEnergy(uint8_t amount);
    void placeProtein();
    void placeProtein(uint8_t amount);
    void placeBacteria();

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

    unsigned int step = 0;

    Game* game;

public:
    // inicjalizatory
    Board(coord width, coord height, Game* game, int bacteriaCount);
    void InitializeRandom(int min, int max);
    void InitializeNeighbour(int recursion, bool includeMiddle);

    void tick();

    void spawnFood(double foodRatio);
    void spawnBacteria(int bacteriaCount);
    size_t getBacteriasNumber();
    // gettery/settery
    inline coord getWidth() const noexcept { return width; }
    inline coord getHeight() const noexcept { return height; }
    inline Hexagon* getHexagon(coord x, coord y) { return (x < 0 || y < 0 || x >= width || y >= height) ? nullptr : &(board[y * width + x]); }
    inline Hexagon* getHexagon(int i) { return (i < 0 || i >= width * height) ? nullptr : &(board[i]); }
    inline int getBacteriaCount() const noexcept { return bacterias.size(); }
    inline BacteriaData& getBacteria(int i) const noexcept { return bacterias[i]; }
    inline BacteriaData& addBacteria() noexcept { return bacterias.emplace_back(); }
    inline const Game* getGame() const noexcept { return game; }

};

