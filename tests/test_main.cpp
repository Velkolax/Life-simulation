#include <catch2/catch_test_macros.hpp>
#include "../board.h"
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