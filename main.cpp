#include "RendererMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RendererMainWindow w(960,540);
    w.show();
    return a.exec();
}
