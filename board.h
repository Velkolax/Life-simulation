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


#define BIG_NUMBER 10000000

typedef int16_t coord;
typedef uint16_t ucoord;
typedef unsigned char uint8;

// NIE ZMIENIAĆ KOLEJNOŚCI WARTOŚCI ANI NIE DODAWAĆ NOWYCH BEZ ZGODY
enum class Resident : uint8
{
    Wall,
    Empty,
    Bacteria,
    Energy,
    Protein
};

inline bool wall(Resident resident) noexcept { return resident == Resident::Wall; };
inline bool empty(Resident resident) noexcept { return resident == Resident::Empty; };
inline bool bacteria(Resident resident) noexcept { return resident == Resident::Bacteria; };
inline bool energy(Resident resident) noexcept {return resident == Resident::Energy;};
inline bool protein(Resident resident) noexcept {return resident == Resident::Protein;};

inline std::mt19937 gen; // generator liczb losowych

class Hexagon; // deklaracje by nie było problemu z mieszaniem kolejności
class Board;
class Player;
class Country;
class Sight;

class Game; // kosmita 👽👽👽

void markAll(std::vector<Hexagon*> hexagons);
void unmarkAll(std::vector<Hexagon*> hexagons);

int calculateIncome(std::vector<Hexagon*> hexagons);

class Hexagon
{
private:
    const coord x;
    const coord y;
    uint8 ownerId; // zakładamy że nie będzie więcej niż 255 graczy
    Resident resident; // enum o wymuszonym rozmiarze bajta

    bool isMarked = false; // do renderowania, oznacza czy heks ma być zaznaczony czy nie
public:
    Hexagon();
    Hexagon(coord x, coord y);
    Hexagon(coord x, coord y, uint8 ownerId, Resident resident);

    inline coord getX() const noexcept { return x; }
    inline coord getY() const noexcept { return y; }
    inline glm::ivec2 getPos() const noexcept {return glm::ivec2(x,y);}
    inline uint8 getOwnerId() const noexcept { return ownerId; }
    inline void setOwnerId(uint8 ownerId) noexcept { this->ownerId = ownerId; }
    inline Resident getResident() const noexcept { return resident; }
    inline void setResident(Resident resident) noexcept { this->resident = resident; }

    int price(Board* board, Resident resident);
    bool isNearWater(Board* board);
    bool bordersPineAndOtherTree(Board* board);

    std::vector<Hexagon*> neighbours(Board* board, int recursion = 0, bool includeSelf = false, std::function<bool(Hexagon*)> filter = nullptr);
    std::vector<Hexagon*> doubleFilterNeighbours(Board* board, int recursion, bool includeSelf, std::function<bool(Hexagon*)> expansionFilter, std::function<bool(Hexagon*)> resultFilter);
    bool isNextToTowerOrCastle(Board* board, uint8 id);
    std::unordered_set<Hexagon*> getAllProtectedAreas(Board* board);

    inline void mark() noexcept { isMarked = true; }
    inline void unmark() noexcept { isMarked = false; }
    inline bool marked() const noexcept { return isMarked; }
};

class Board
{
private:
    const coord width;
    const coord height;
    std::vector<Hexagon> board;

    unsigned int step = 0;

    Game* game;

public:
    // inicjalizatory
    Board(coord width, coord height, Game* game);
    void InitializeRandom(int min, int max);
    void InitializeNeighbour(int recursion, bool includeMiddle);
    void spawnTrees(double treeRatio);
    void spawnFood(double foodRatio);


    void spawnBacteria(int bacteriaCount);
    int getBacteriasNumber();
    // gettery/settery
    inline coord getWidth() const noexcept { return width; }
    inline coord getHeight() const noexcept { return height; }
    inline Hexagon* getHexagon(coord x, coord y) { return (x < 0 || y < 0 || x >= width || y >= height) ? nullptr : &(board[y * width + x]); }
    inline Hexagon* getHexagon(int i) { return (i < 0 || i >= width * height) ? nullptr : &(board[i]); }


    inline const Game* getGame() const noexcept { return game; }

    void propagateTrees();
};

