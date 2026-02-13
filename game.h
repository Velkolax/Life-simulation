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
    std::unordered_map<int, bool> keysLast;
    std::unordered_map<int, bool> buttons;
    std::unordered_map<int, bool> buttonsLast;
    std::unordered_map<int,bool> toggles;
    int cursorX=0,cursorY=0;
    int scrollDelta = 0;

    bool isDown(int key){return keys[key];}
    bool isPressed(int key){return keys[key] && !keysLast[key];}
    bool isReleased(int key){return !keys[key] && keysLast[key];}
    bool isToggled(int key){return toggles[key];}

    bool isButtonDown(int btn) { return buttons[btn]; }
    bool isButtonPressed(int btn) { return buttons[btn] && !buttonsLast[btn]; }
    bool isButtonReleased(int btn) { return !buttons[btn] && buttonsLast[btn]; }

    void setKeyState(int key,bool pressed)
    {
        keysLast[key]=keys[key];
        keys[key] = pressed;
        if (keys[key] && !keysLast[key]) toggles[key] = !toggles[key];
    }

    void setButtonState(int btn, bool pressed) {
        buttons[btn] = pressed;
    }

    void update()
    {
        keysLast = keys;
        buttonsLast = buttons;
        scrollDelta = 0;
    }
};

class BacteriaWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    BacteriaWidget(QWidget *parent = nullptr);
    ~BacteriaWidget();
    Input* getInput(){return &input;}
    void setSubsteps(int val){substeps=val;}
    SpriteRenderer *Renderer;
    TextRenderer *Text;
    SimulationEngine *engine;

    signals:
    void statsUpdated(int count);
    void stepInfoUpdated(int count);
    void speciesInfoUpdated(int count);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void tick();
    void restart();
    Input input;
    Board *board;


    QTimer *timer;
    int Width,Height;
    int substeps=1;

};

#endif