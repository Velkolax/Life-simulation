#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "sprite_renderer.h"
#include <iostream>
#include <ranges>
#include "board.h"
#include "text_renderer.h"

struct Input
{
    bool keys[1024] = {false};
    bool keysLast[1024] = {false};
    bool toggles[1024] = {false};

    void update(GLFWwindow* window)
    {
        for (int i=0;i<1024;i++)
        {
            keysLast[i] = keys[i];
            keys[i] = glfwGetKey(window,i) == GLFW_PRESS;
            if (keys[i] && !keysLast[i]) toggles[i] = !toggles[i];
        }
    }
    bool isDown(int key){return keys[key];}
    bool isPressed(int key){return keys[key] && !keysLast[key];}
    bool isReleased(int key){return !keys[key] && keysLast[key];}
    bool isToggled(int key){return toggles[key];}
    void setToggle(int key, bool state){toggles[key]=state;}
};

class Game
{
    public:
        bool mousePressed = false, fullscreen=false;
        float cursorPosX,cursorPosY;
        int scroll = 0;
        unsigned int Width, Height;
        Board* board;
        SimulationEngine* engine;
        TextRenderer  *Text;
    SpriteRenderer* Renderer;
        GLFWwindow* window;
        Game();
        ~Game();
        void Run();
        void ProcessInput();
        void restart();
        void tick();
        void Resize(int width, int height);
        void peekBacteria();
        void lockBacteria();
        void Render();
        inline Input getInput(){return input;}
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    private:
        Input input;
};




#endif