#include "QtMain/RendererMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RendererMainWindow w(960,540);
    w.show();
    return a.exec();
}
