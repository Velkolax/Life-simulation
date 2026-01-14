#include "board.h"
#include <unordered_set>
#include <cstring>
#include <set>
#include <algorithm>

#include "game.h"
#include "simulation_engine.h"


Hexagon::Hexagon() : x(0), y(0), resident(Resident::Wall){}

Hexagon::Hexagon(coord x, coord y) : x(x), y(y), resident(Resident::Wall){}

Hexagon::Hexagon(coord x, coord y, Resident resident) : x(x), y(y), resident(resident)
{}

Board::Board(coord width, coord height, Game* game, int bacteriaCount) : width(width), height(height), game(game)
{
    board.reserve(width * height);
    for (coord y = 0; y < height; y++)
    {
        for (coord x = 0; x < width; x++)
        {
            board.emplace_back(x, y);
        }
    }
    bacterias.reserve(width*height);
    vacantSlots.reserve(width*height);
}


void Board::InitializeNeighbour(int recursion, bool includeMiddle)
{
    Hexagon* middle = getHexagon(getWidth() / 2, getHeight() / 2);
    if(!middle) return;
    auto neighbours = middle->neighbours(this, recursion, includeMiddle);
    for(Hexagon* hex : neighbours)
    {
        hex->placeEmpty();
    }
}

void Board::InitializeRandom(int min, int max)
{
    std::uniform_int_distribution<int> randN(min, max);
    int n = randN(gen);

    Hexagon* middle = getHexagon(getWidth() / 2, getHeight() / 2);
    if(!middle) return;

    std::unordered_set<Hexagon*> addableS = { middle };
    std::vector<Hexagon*> addableV = { middle };

    while(n > 0 && !addableV.empty())
    {
        std::uniform_int_distribution<int> randomAddable(0, addableV.size() - 1);
        int index = randomAddable(gen);
        Hexagon* hex = addableV[index];
        addableS.erase(hex);
        addableV[index] = addableV.back();
        addableV.pop_back();
        hex->placeEmpty();
        auto neighbours = hex->neighbours(this, 0, false, [](Hexagon* h) { return wall(h->getResident()); });
        for(Hexagon* neighbour : neighbours)
        {
            if(!addableS.count(neighbour))
            {
                addableS.insert(neighbour);
                addableV.push_back(neighbour);
            }
        }
        n--;
    }
}


void Board::tick()
{
    step++;

    struct Point
    {
        coord x;
        coord y;
    };
    int count = getBacteriaCount();
    std::vector<uint32_t> idsBuffer;
    idsBuffer.reserve(count);
    std::vector<Point> points;
    points.reserve(count);
    std::vector<float> hostInBuffer;
    hostInBuffer.reserve(count*INPUT);

    size_t total = board.size();
    for(int i = 0; i < total; i++)
    {
        if(bacteria(board[i].getResident()))
        {
            int32_t index = board[i].getData().bacteriaIndex;
            BacteriaData& b = getBacteria(index);
            if(!b.age) b.age = 1;
            if(step % 1000 == 0)
            {
                b.getOlder(this, board[i].getX(), board[i].getY());
                if(!bacteria(board[i].getResident())) continue; // Bakteria zginęła przez stary wiek
            }
            if(step % b.speed == 0)
            {
                idsBuffer.push_back(index);
                points.push_back({board[i].getX(), board[i].getY()});
                size_t currentIdx = hostInBuffer.size();
                hostInBuffer.resize(currentIdx + INPUT);
                b.addToBuffer(this, &hostInBuffer[currentIdx], board[i].getX(), board[i].getY());
            }
        }
    }
    if (idsBuffer.empty()) return;
    std::vector<float> hostOutBuffer; hostOutBuffer.resize(count*OUTPUT);
    game->engine->Process(idsBuffer.size(), idsBuffer.data(), hostInBuffer.data(), hostOutBuffer.data());
    for(int i = 0; i < idsBuffer.size(); i++)
    {
        if(!bacteria(getHexagon(points[i].x, points[i].y)->getResident())) continue; // Bakteria zginęła i teraz na heksie jest pustka
        BacteriaData& b = getBacteria(idsBuffer[i]);
        if(!b.age) continue; // Nowonarodzona zastąpiła starą martwą ale nie powinna wykonywać jej ruchów
        float* currentOutput = &hostOutBuffer[i*OUTPUT];
        memcpy(b.memory, currentOutput, MEMORY_SIZE * sizeof(float));
        b.execute(this, currentOutput + MEMORY_SIZE, points[i].x, points[i].y);
    }


    counter++;
    if (counter % 100 == 0) spawnFood(0.05);
    resourcesMerge();
}

void Board::resourcesMerge()
{
    for (int y = 0; y < getHeight(); y++)
    {
        for (int x = 0; x < getWidth(); x++)
        {
            Hexagon* hex = getHexagon(x, y);
            if (resource(hex->getResident()))
            {
                uint8_t hexR = hex->getData().acid.amount; // struktura wewnętrzna wszystkich zasobów jest taka sama więc pobierzmy ilość jako z kwasu 
                auto& directions = (x & 1) ? oddDirections2l : evenDirections2l;
                for(auto& [dx,dy] : directions)
                {
                    Hexagon* h = getHexagon(hex->getX() + dx, hex->getY() + dy);
                    if (h != nullptr && h->getResident() == hex->getResident())
                    {
                        uint8_t hR = h->getData().acid.amount; // tak samo jak wyżej
                        if(hexR + hR <= MAX_STORED_VALUE)
                        {
                            h->getData().acid.amount += hR;
                            h->placeEmpty();
                        }
                    }
                }
            }
        }
    }
}

size_t Board::getProteinCount()
{
    size_t proteinCount = 0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && protein(hex->getResident()))
        {
            proteinCount += hex->getData().protein.amount;
        }
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            proteinCount += bac.protein;
        }
    }
    return proteinCount;
}

int Board::getHighestAge()
{
    int highestAge = 0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            if (bac.age>highestAge) highestAge = bac.age;
        }
    }
    return highestAge;
}

int Board::getLowestAge()
{
    int lowestAge = INT32_MAX;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            if (bac.age<lowestAge) lowestAge = bac.age;
        }
    }
    return lowestAge;
}

double Board::getAvgEnergy()
{
    double energySum = 0.0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            energySum += bac.energy;
        }
    }
    return energySum / getAliveBacteriaCount();
}



float Board::getFailureRatio()
{
    float failureSum = 0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            if (bac.lastAction==Action::MoveFailure || bac.lastAction==Action::AttackFailure || bac.lastAction==Action::BreedFailure || bac.lastAction==Action::EatFailure) failureSum++;
        }
    }
    return failureSum / (float)getAliveBacteriaCount() * 100.0f;
}

float Board::getActionPercentage(Action a)
{
    float actionSum = 0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (hex != nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = getBacteria(res.bacteriaIndex);
            if (bac.lastAction==a) actionSum++;
        }
    }
    return actionSum / (float)getAliveBacteriaCount() * 100.0f;
}


void Hexagon::placeWall()
{
    resident = Resident::Wall;
}

void Hexagon::placeEmpty()
{
    resident = Resident::Empty;
}

void Hexagon::placeAcid()
{
    resident = Resident::Acid;
    data.acid.amount = std::uniform_int_distribution<uint8_t>(1, 10)(gen);
}

void Hexagon::placeAcid(uint8_t amount)
{
    resident = Resident::Acid;
    data.acid.amount = amount;
}

void Hexagon::placeEnergy()
{
    resident = Resident::Energy;
    data.energy.amount = std::uniform_int_distribution<uint8_t>(50, 100)(gen);
}

void Hexagon::placeEnergy(uint8_t amount)
{
    resident = Resident::Energy;
    data.energy.amount = amount;
}

void Hexagon::placeProtein()
{
    resident = Resident::Protein;
    data.protein.amount = std::uniform_int_distribution<uint8_t>(1, 10)(gen);
}

void Hexagon::placeProtein(uint8_t amount)
{
    resident = Resident::Protein;
    data.protein.amount = amount;
}

void Hexagon::placeBacteria(Board* board)
{
    resident = Resident::Bacteria;
    int32_t newId = board->addBacteria();
    data.bacteriaIndex = newId;
    board->getBacteria(newId).randomize();
}

void Hexagon::placeChild(Board* board, BacteriaData& mom)
{
    resident = Resident::Bacteria;
    int32_t newId = board->addBacteria();
    data.bacteriaIndex = newId;
    board->getBacteria(newId).cross(mom);
}

void Hexagon::placeBacteria(Board* board, uint32_t id)
{
    resident = Resident::Bacteria;
    data.bacteriaIndex = id;
}

/*bool Hexagon::isNearWater(Board *board)
{
    return (neighbours(board, 0, false, [](Hexagon* h) { return wall(h->resident); })).size() > 0;
}*/

void Board::spawnFood(double foodRatio)
{
    std::uniform_int_distribution<int> dist(0,100);
    int count = board.size();
    std::vector<int> range(count);
    std::iota(range.begin(), range.end(), 0);
    std::erase_if(range,[this](int i){return this->board[i].getResident()!=Resident::Empty;});

    std::shuffle(range.begin(),range.end(),gen);
    for (int i=0;i<range.size()*foodRatio;i++)
    {
        int index = dist(gen);
        if (empty(board[range[i]].getResident()))
        {
            if (index<60) board[range[i]].placeEnergy();
            else if (index<95) board[range[i]].placeProtein();
            else board[range[i]].placeAcid();
        }

    }
}

void Board::spawnBacteria(int bacteriaCount)
{
    int count = board.size();
    std::vector<int> range(count);
    std::iota(range.begin(), range.end(), 0);
    std::erase_if(range,[this](int i){return this->board[i].getResident()!=Resident::Empty;});

    std::shuffle(range.begin(),range.end(),gen);
    for (int i = 0; i < range.size(); i++)
    {
        if (i < bacteriaCount)
        {
            //int x = board[range[i]].getX();
            //int y = board[range[i]].getY();
            board[range[i]].placeBacteria(this);
        }
    }
}

int Board::getAliveBacteriaCount()
{
    int counter=0;
    for (int i=0;i<getHeight()*getWidth();i++)
    {
        Hexagon *hex = getHexagon(i);
        if (bacteria(hex->getResident())) counter++;
    }
    return counter;
}

std::vector<std::pair<coord, coord>> evenDirections =
{
    { 0, -1}, // górny
    {-1, -1}, // lewy górny
    {-1,  0}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  0}, // prawy dolny
    { 1, -1}  // prawy górny
};

std::vector<std::pair<coord, coord>> oddDirections =
{
    { 0, -1}, // górny
    {-1,  0}, // lewy górny
    {-1,  1}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  1}, // prawy dolny
    { 1,  0}  // prawy górny
};

void addNeighboursLayer(Board* board, std::unordered_set<Hexagon*>& visited, std::vector<Hexagon*>& hexagons, int recursion, std::function<bool(Hexagon*)> filter)
{
    if(hexagons.size() == 0) return;
    std::vector<Hexagon*> newHexagons;
    newHexagons.reserve(hexagons.size() * 6);

    for(auto hexagon : hexagons)
    {
        coord x = hexagon->getX();
        coord y = hexagon->getY();
        auto& directions = (x % 2 == 0) ? evenDirections : oddDirections;

        for (auto [dx, dy] : directions)
        {
            Hexagon* hex = board->getHexagon(x + dx, y + dy); // getHexagon() robi sprawdzanie zakresów
            if(hex != nullptr && filter(hex) && !visited.count(hex))
            {
                visited.insert(hex);
                newHexagons.push_back(hex);
            }
        }
    }
    if(recursion > 0) addNeighboursLayer(board, visited, newHexagons, recursion - 1, filter);
}

std::vector<Hexagon*> Hexagon::neighbours(Board *board, int recursion, bool includeSelf, std::function<bool(Hexagon*)> filter)
{
    if (!filter) filter = [](Hexagon*) { return true; };
    std::unordered_set<Hexagon*> visited = { this };
    std::vector<Hexagon*> newHexagons = { this };
    addNeighboursLayer(board, visited, newHexagons, recursion, filter);
    if(!includeSelf) visited.erase(this);
    return std::vector<Hexagon*>(visited.begin(), visited.end());
}

void doubleFilterAddNeighboursLayer(Board* board, std::unordered_set<Hexagon*>& visited, std::vector<Hexagon*>& hexagons, int recursion, std::function<bool(Hexagon*)> expansionFilter, std::function<bool(Hexagon*)> resultFilter)
{
    if(hexagons.size() == 0) return;
    std::vector<Hexagon*> newHexagons;
    newHexagons.reserve(hexagons.size() * 6);

    for(auto hexagon : hexagons)
    {
        coord x = hexagon->getX();
        coord y = hexagon->getY();
        auto& directions = (x % 2 == 0) ? evenDirections : oddDirections;

        for (auto [dx, dy] : directions)
        {
            Hexagon* hex = board->getHexagon(x + dx, y + dy); // getHexagon() robi sprawdzanie zakresów
            if(hex != nullptr && !visited.count(hex) && expansionFilter(hex))
            {
                newHexagons.push_back(hex);
                if(resultFilter(hex)) visited.insert(hex);
            }
        }
    }
    if(recursion > 0) doubleFilterAddNeighboursLayer(board, visited, newHexagons, recursion - 1, expansionFilter, resultFilter);
}

// Działa podobnie do neighbours() ale ma dwa filtry: rozrostu (które heksy analizujemy przy następnej iteracji) i wyniku (co zostanie zwrócone)
std::vector<Hexagon*> Hexagon::doubleFilterNeighbours(Board *board, int recursion, bool includeSelf, std::function<bool(Hexagon*)> expansionFilter, std::function<bool(Hexagon*)> resultFilter)
{
    if (!expansionFilter) expansionFilter = [](Hexagon*) { return true; };
    if (!resultFilter) resultFilter = [](Hexagon*) { return true; };
    std::unordered_set<Hexagon*> visited = { this };
    std::vector<Hexagon*> newHexagons = { this };
    doubleFilterAddNeighboursLayer(board, visited, newHexagons, recursion, expansionFilter, resultFilter);
    if(!includeSelf) visited.erase(this);
    return std::vector<Hexagon*>(visited.begin(), visited.end());
}


void addNeighboursLayerWithBorder(Board* board, std::unordered_set<Hexagon*>& visited, std::unordered_set<Hexagon*>& border, std::vector<Hexagon*>& hexagons, int recursion, std::function<bool(Hexagon*)> filter)
{
    if(hexagons.size() == 0) return;
    std::vector<Hexagon*> newHexagons;
    newHexagons.reserve(hexagons.size() * 6);

    for(auto hexagon : hexagons)
    {
        coord x = hexagon->getX();
        coord y = hexagon->getY();
        auto& directions = (x % 2 == 0) ? evenDirections : oddDirections;

        for (auto [dx, dy] : directions)
        {
            Hexagon* hex = board->getHexagon(x + dx, y + dy); // getHexagon() robi sprawdzanie zakresów
            if(hex != nullptr && !visited.count(hex) && !border.count(hex))
            {
                if(filter(hex))
                {
                    visited.insert(hex);
                    newHexagons.push_back(hex);
                }
                else
                {
                    border.insert(hex);
                }
            }
        }
    }
    if(recursion > 0) addNeighboursLayerWithBorder(board, visited, border, newHexagons, recursion - 1, filter);
}

