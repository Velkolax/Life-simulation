#include "game.h"
#include <QApplication>
#include <QSurfaceFormat>

#include "game_configdata.h"
#include "main_window.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4,5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    GameConfigData::setConfigDataFromFile("config.txt");
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc,argv);

    MainWindow w;
    w.setWindowTitle("Evolution simulator");
    w.resize(1200,800);

    w.show();
    return a.exec();
}

