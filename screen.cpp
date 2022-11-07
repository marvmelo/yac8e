#include <iostream>
#include <raylib.h>
#define WIDTH 64
#define HEIGHT 32
#define FACTOR 10

using namespace std;

class Screen
{
private:
    int screenBuffer[WIDTH][HEIGHT];
public:
    Screen(/* args */);
    ~Screen();
    void Render();
};

Screen::Screen(/* args */)
{
}

Screen::~Screen()
{
}

void Screen::Render()
{
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            Color pColor = screenBuffer[i][j] ? WHITE : BLACK;
            DrawRectangle(i*FACTOR, j*FACTOR, FACTOR, FACTOR, pColor);
        }
    }
}
