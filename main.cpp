#include "RenderrerMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RenderrerMainWindow w(960,540);
    w.show();
    return a.exec();
}
