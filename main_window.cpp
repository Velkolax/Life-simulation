//
// Created by tk2 on 2/12/26.
//

#include "main_window.h"
#include "game_configdata.h"

MainWindow::MainWindow()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    bacteriaWidget = new BacteriaWidget(this);
    mainLayout->addWidget(bacteriaWidget,4);

    QVBoxLayout *sidePanel = new QVBoxLayout();
    mainLayout->addLayout(sidePanel,1);

    // QPushButton *btnReset = new QPushButton("Reset Simulation",this);
    // sidePanel->addWidget(btnReset);
    // connect(btnReset, &QPushButton::clicked, [this]()
    // {
    //    qDebug() << "ZRESTARTOWANA!";
    // });

    sidePanel->addWidget(new QLabel("Simulation Speed: ",this));
    QSlider *speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1,30);
    speedSlider->setValue(GameConfigData::getInt("substeps"));
    sidePanel->addWidget(speedSlider);

    connect(speedSlider, &QSlider::valueChanged, [this](int val)
    {
        bacteriaWidget->setSubsteps(val);
        //qDebug() << "Odczytano: " << val;
    });

    statsLabel = new QLabel("Bacteria Count: 0",this);
    sidePanel->addWidget(statsLabel);
    connect(bacteriaWidget,&BacteriaWidget::statsUpdated, [this](int count)
    {
        statsLabel->setText(QString("Bacteria Count: %1").arg(count));
    });

    stepInfoLabel = new QLabel("Step: 0",this);
    sidePanel->addWidget(stepInfoLabel);
    connect(bacteriaWidget,&BacteriaWidget::stepInfoUpdated,[this](int count)
    {
       stepInfoLabel->setText(QString("Step: %1").arg(count));
    });

    speciesInfoLabel = new QLabel("Species Left: 0",this);
    sidePanel->addWidget(speciesInfoLabel);
    connect(bacteriaWidget,&BacteriaWidget::speciesInfoUpdated,[this](int count)
    {
       speciesInfoLabel->setText(QString("Species Left: %1").arg(count));
    });

    sidePanel->addStretch();
}

MainWindow::~MainWindow()
{

}
