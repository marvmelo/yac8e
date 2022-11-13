#include<iostream>
#include <fstream>
#include "keyboard.h"
#include "chip8.h"
#include "timer.h"
#define WIDTH 64
#define HEIGHT 32
#define FACTOR 20
#define CLOCK 60.0f

using namespace std;

int main()
{
    ifstream ROMFile;
    ROMFile.open("roms/Life [GV Samways, 1980].ch8", ios::binary|ios::ate);
    int size = (int)ROMFile.tellg();
    ROMFile.seekg(0);
    char ROMArray[size];
    unsigned char ROMArrayU[size];
    ROMFile.read(ROMArray, size);
    ROMFile.close();
    InitWindow(WIDTH*FACTOR, HEIGHT*FACTOR, "Yet Another Chip-8 Emulator");
    SetTargetFPS(60);
    Chip8 chip8(ROMArray, size);
    int keysPressed[16] = {};
    while (!WindowShouldClose())
    {
        double startTime = GetTime();
        CheckKeys(keysPressed);
        chip8.Cycle(keysPressed);    
        WaitCycle(startTime, 1.0f/CLOCK);
    }
    CloseWindow();
    return 0;
}