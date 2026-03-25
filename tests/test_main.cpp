#include <catch2/catch_test_macros.hpp>
#include "../board.h"
#include "../BacteriaData.h"
#include "../game_configdata.h"

TEST_CASE("Board initialization","[board]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(50,60,nullptr,10);

    SECTION("Board dimensions are correct")
    {
        REQUIRE(board.getWidth() == 50);
        REQUIRE(board.getHeight() == 60);
    }
}

TEST_CASE("Hexagon neighbor logic", "[grid]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(20,20,nullptr,0);

    SECTION("Even row neighbors")
    {
        Hexagon *h = board.getHexagon(2,2);
        auto neighbors = h->neighbours(&board,0,false);
        REQUIRE(neighbors.size() == 6);
    }
}

TEST_CASE("Bacterium metabolism and death", "[bacteria]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    Hexagon *hex = board.getHexagon(5,5);
    hex->placeBacteriaC(&board,1);

    int id = hex->getData().bacteriaIndex;
    BacteriaData& data = board.getBacteria(id);

    SECTION("Bacterium gains energy from eating")
    {
        data.energy = 10;
        Hexagon* foodHex = board.getHexagon(5,6);
        foodHex->placeEnergy(20);

        float mockData[] = { 0.0f, 1.0f, 0.0f };
        data.eat(&board,foodHex,mockData,5,5);
        REQUIRE(data.energy>10);
        REQUIRE(foodHex->getResident() == Resident::Empty);
    }
}

TEST_CASE("Bacterium breeding logic","[bacteria]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    Hexagon *momHex = board.getHexagon(5,5);
    momHex->placeBacteriaC(&board,1);
    int momId = momHex->getData().bacteriaIndex;
    BacteriaData& momData = board.getBacteria(momId);

    Hexagon* dadHex = board.getHexagon(5,4);
    dadHex->placeBacteriaC(&board,1);
    Hexagon* childHex = board.getHexagon(5,3);
    childHex->placeEmpty();

    SECTION("Successful breeding")
    {
        momData.energy = 100;
        momData.protein = 100;

        float mockData[] = {0.1f,0.5f,0.3f,0.3f};
        int result = momData.breed(&board,dadHex,mockData,5,5);
        std::cout << "RESULT: " << result;
        auto neighbors = momHex->neighbours(&board,2,false);
        int bacteriaCount = 0;
        for (auto n : neighbors)
        {
            if (bacteria(n->getResident())) bacteriaCount++;
        }
        REQUIRE(bacteriaCount==2);
    }
}

