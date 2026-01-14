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
    bool sPressed=false,sToggled=false;
    bool isPaused = true;
    bool isBacteriaSelected = false;
    bool spacePressed = false;
    Board* board;
    SimulationEngine* engine;
    TextRenderer  *Text;
    GLFWwindow* window;
    Game();
    ~Game();
    void Init();
    void Run();
    void ProcessInput();
    void Update();
    void Resize(int width, int height);
    void Render();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    std::map<int,bool> clickedMovingKeys{
        {GLFW_KEY_W,false},
        {GLFW_KEY_A,false},
        {GLFW_KEY_S,false},
        {GLFW_KEY_D,false}
    };
};




#endif