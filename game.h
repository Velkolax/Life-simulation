#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "sprite_renderer.h"
#include <iostream>
#include <ranges>


#include "board.h"
#include "text_renderer.h"







class Game
{
public:
    int                     pressedKey = -1;
    bool                    mousePressed = false;
    float                   cursorPosX;
    float                   cursorPosY;
    int                     scroll = 0;
    unsigned int            Width, Height;
    bool                    enterPressed = false;
    bool isPaused = true;
    bool isBacteriaSelected = false;
    bool spacePressed = false;
    Board* board;
    SimulationEngine* engine;
    TextRenderer  *Text;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float dt);
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




#endif