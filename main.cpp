#include<iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "chip8.h"
#define WIDTH 64
#define HEIGHT 32
#define FACTOR 10

using namespace std;

int main()
{
    ifstream ROMFile;
    ROMFile.open("roms/Pong (1 player).ch8", ios::binary|ios::ate);
    int size = (int)ROMFile.tellg();
    ROMFile.seekg(0);
    char ROMArray[size];
    unsigned char ROMArrayU[size];
    ROMFile.read(ROMArray, size);
    ROMFile.close();
    InitWindow(WIDTH*FACTOR, HEIGHT*FACTOR, "Yet Another Chip-8 Emulator");
    Chip8 chip8(ROMArray, size);
    while (1)
    {
        chip8.Cycle();    
        this_thread::sleep_for(chrono::nanoseconds(16));
    }
    return 0;
}