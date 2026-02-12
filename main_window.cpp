//
// Created by tk2 on 2/12/26.
//

#include "main_window.h"

MainWindow::MainWindow()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    bacteriaWidget = new BacteriaWidget(this);
    mainLayout->addWidget(bacteriaWidget,4);

    QVBoxLayout *sidePanel = new QVBoxLayout();
    mainLayout->addLayout(sidePanel,1);

    QPushButton *btnReset = new QPushButton("Reset Simulation",this);
    sidePanel->addWidget(btnReset);
    connect(btnReset, &QPushButton::clicked, [this]()
    {
       qDebug() << "ZRESTARTOWANA!";
    });

    sidePanel->addWidget(new QLabel("Simulation Speed: ",this));
    QSlider *speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1,10);
    speedSlider->setValue(1);
    sidePanel->addWidget(speedSlider);

    connect(speedSlider, &QSlider::valueChanged, [this](int val)
    {
       qDebug() << "Odczytano: " << val;
    });

    statsLabel = new QLabel("Bacteria Count: 0",this);
    sidePanel->addWidget(statsLabel);
    sidePanel->addStretch();
}
