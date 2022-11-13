#include <iostream>
#include <raylib.h>
#include "misc.h"
#define WIDTH 64
#define HEIGHT 32
#define FACTOR 20
#define TRUE 1
#define FALSE 0

using namespace std;

class Screen
{
private:
    int screenBuffer[WIDTH][HEIGHT];
public:
    Screen();
    int Draw(unsigned char*, int, int, int);
    void Render();
    void Clear();
};

Screen::Screen()
{
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            screenBuffer[i][j] = 0;
        }
    }
}

void Screen::Clear()
{
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            screenBuffer[i][j] = 0;
        }
    }
}

void Screen::Render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            if (screenBuffer[i][j])
            {
            DrawRectangle(i*FACTOR, j*FACTOR, FACTOR, FACTOR, WHITE);
            }
        }
    }
    EndDrawing();
}

int Screen::Draw(unsigned char *spritesPtr, int X, int Y, int nibble)
{
    int colisionFlag = FALSE;
    for (int nByte = 0; nByte < nibble; nByte++)
    {
        unsigned char byte = spritesPtr[nByte];
        int currentY = (Y + nByte) % HEIGHT;
        for (int i = 0; i < 8; i++)
        {
            int currentX = (X + i) % WIDTH;
            int currentBit = GetBit(byte, 7-i);
            screenBuffer[currentX][currentY] ^= currentBit;
            colisionFlag = (screenBuffer[currentX][currentY] & currentBit) ? TRUE : FALSE;
        }
    }
    return colisionFlag;
}
