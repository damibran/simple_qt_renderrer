#include "mainwindow.h"

#include <QtWidgets/qapplication.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    MainWindow window(400, 300);

    window.show();

    return app.exec();
}
