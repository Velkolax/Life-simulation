#include "game.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4,5);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc,argv);

    BacteriaWidget w;
    w.setWindowTitle("Evolution simulator");
    w.resize(1000,800);

    w.show();
    return a.exec();
}

