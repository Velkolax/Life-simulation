#ifndef GAME_H
#define GAME_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QKeyEvent>
#include "sprite_renderer.h"
#include <iostream>
#include <ranges>
#include "board.h"
#include "text_renderer.h"

struct Input
{
    std::unordered_map<int, bool> keys;
    std::unordered_map<int, bool> keysPressed;
    std::unordered_map<int,bool> toggles;

    bool isDown(int key){return keys[key];}
    bool isPressed(int key)
    {
        bool res = keysPressed[key];
        keysPressed[key] = false;
        return res;
    }
    bool isToggled(int key){return toggles[key];}

    void setKeyState(int key,bool pressed)
    {
        if (pressed && !keys[key])
        {
            keysPressed[key] = true;
            toggles[key] = !toggles[key];
        }
        keys[key]=pressed;
    }
};

class BacteriaWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    BacteriaWidget(QWidget *parent = nullptr);
    ~BacteriaWidget();
    SpriteRenderer *Renderer;
    TextRenderer *Text;
    SimulationEngine *engine;
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void tick();
    void restart();
    Input input;
    Board *board;


    QTimer *timer;
    std::set<int> pressedKeys;
    int Width,Height;
};



// class Game
// {
//     public:
//         bool mousePressed = false, fullscreen=false;
//         float cursorPosX,cursorPosY;
//         int scroll = 0;
//         unsigned int Width, Height;
//         Board* board;
//         SimulationEngine* engine;
//         TextRenderer  *Text;
//     SpriteRenderer* Renderer;
//         GLFWwindow* window;
//         Game();
//         ~Game();
//         void Run();
//         void ProcessInput();
//         void restart();
//         void tick();
//         void Resize(int width, int height);
//         void peekBacteria();
//         void lockBacteria();
//         void Render();
//         inline Input getInput(){return input;}
//         static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//         static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//         static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//         static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//     private:
//         Input input;
// };




#endif