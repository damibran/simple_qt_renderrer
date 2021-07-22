#include "mainwindow.h"

#include <QtWidgets/qapplication.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    MainWindow window(300,300);
    window.show();

    QTimer timer;
    timer.setInterval(16);

    QObject::connect(&timer,SIGNAL(timeout()),&window,SLOT(screen_refresh()));

    timer.start(0);

    return app.exec();
}
