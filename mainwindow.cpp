#include"mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(int wr,int hr,QWindow *parent)
    : QWindow(parent)
    , m_backingStore(new QBackingStore(this))
    ,screen(wr,hr)
    ,scene(screen)
{
    setGeometry(100, 100, wr*3, hr*3);
    m_width = wr;
    m_height = hr;

}

void MainWindow::screen_refresh()
{
    if (!isExposed())
        return;

    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);
    QPen screenPen;

    //configure painter
    painter.setWindow(0,0,m_width, m_height);

    painter.fillRect(0, 0,m_width, m_height,QBrush(QColor(255,255,255)));

    //here must be rendering implemented
    screen.clearScreen();
    scene.updateScene();

   for(int y =0 ;y<m_height;++y)
   {
       for(int x=0;x<m_width;++x)
       {
           screenPen.setColor(screen.getColorAtIndex(y*m_width + x));
           painter.setPen(screenPen);

           painter.drawPoint(x,y);
       }
   }

    painter.end();

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        screen_refresh();
        return true;
    }
    return QWindow::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==16777216)
        this->close();
}

void MainWindow::exposeEvent(QExposeEvent *)
{
    //if (isExposed())
    //    screen_refresh();
}

void MainWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    QSize size = resizeEvent->size();
    m_backingStore->resize(size);
}
