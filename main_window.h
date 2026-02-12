//
// Created by tk2 on 2/12/26.
//

#ifndef LIFE_SIM_MAIN_WINDOW_H
#define LIFE_SIM_MAIN_WINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

#include "game.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
private:
    BacteriaWidget *bacteriaWidget;
    QLabel *statsLabel,*stepInfoLabel,*speciesInfoLabel;
};




#endif //LIFE_SIM_MAIN_WINDOW_H
