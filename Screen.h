#ifndef SCREEN_H
#define SCREEN_H
#include<QColor>
#include<vector>
#include<glm/glm.hpp>

class Screen
{
public:
    const int XMAX;
    const int YMAX;
    Screen(int mx,int my):XMAX(mx),YMAX(my)
    {
        colorBuffer.resize(XMAX*YMAX);
    }

    void put_point(int a, int b, glm::vec3 color)
    {
        colorBuffer[(YMAX - b) * XMAX + a] = color;
    }

    QColor getColorAtIndex(int ndx)
    {
        return QColor(colorBuffer[ndx].r,colorBuffer[ndx].g,colorBuffer[ndx].b);
    }

    void clearScreen()
    {
        for(int i=0;i<colorBuffer.size();++i)
        {
            colorBuffer[i]=glm::vec3(100);
        }
    }
private:
    std::vector<glm::vec3> colorBuffer;
};

#endif // SCREEN_H
