#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <glm/glm.hpp>
#include "Screen.h"
#include "Scene.h"

class MainWindow : public QWindow
{
    Q_OBJECT
public:
    explicit MainWindow(int wr,int hr,QWindow *parent = 0);
    void put_point(int a,int b,glm::vec3 color);

public slots:
    void screen_refresh();

protected:
    bool event(QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QBackingStore *m_backingStore;
    int m_width;
    int m_height;
    Screen screen;
    Scene scene;
};
#endif // MAINWINDOW_H
