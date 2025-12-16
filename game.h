#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "sprite_renderer.h"
#include <iostream>
#include <ranges>


#include "board.h"
#include "text_renderer.h"



// Represents the current state of the game
enum class GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};


// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;	
    int                     pressedKey = -1;
    bool                    mousePressed;
    float                   cursorPosX;
    float                   cursorPosY;
    int                     scroll = 0;
    unsigned int            Width, Height;
    bool                    enterPressed = false;

    Board *board;
    TextRenderer  *Text;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float dt);
    int GetSelectedCastleReserves();
    int GetSelectedCastleIncome();
    void RefreshSprites();
    void RefreshOutline();
    void Update(float dt);
    void Resize(int width, int height);
    //void Tick();
    void Render();
    std::map<int,bool> clickedMovingKeys{
        {GLFW_KEY_W,false},
        {GLFW_KEY_A,false},
        {GLFW_KEY_S,false},
        {GLFW_KEY_D,false}
    };
};

void executeActions(Board* board, char* actions, uint8 actionsNumber);




#endif