#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <raylib.h>
#include "screen.h"
#include "audiodevice.h"
#define WIDTH 64
#define HEIGHT 32
#define FACTOR 10
#define TRUE 1
#define FALSE 0

using namespace std;

class Chip8
{
private:
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned char delayTimer;
    unsigned char soundTimer;
    unsigned short int I;
    unsigned short int PC;
    unsigned char SP;
    unsigned short int stack[16];

    Screen screen;
    AudioDevice audioDevice;

    KeyboardKey keys[16];

public:
    Chip8(/* args */);
    unsigned short int Fetch();
    void DecodeExecute(unsigned short int);
    void ExecZeroPrefix(unsigned short int);
    void ExecSet(unsigned short int);
    void SkipIfKeyDown(unsigned short int);
    void ExecFPRefix(unsigned short int);
    void WaitKeyPress(unsigned short int);
    void StoreBCD();
    void Cycle();
};

Chip8::Chip8(/* args */)
{
}

unsigned short int Chip8::Fetch()
{
    unsigned short int opcode;
    opcode = memory[PC];
    opcode = opcode << 8;
    opcode = opcode + memory[PC+1];
    PC = PC + 2;
    return opcode;
}

void Chip8::ExecZeroPrefix(unsigned short int opcode)
{
    if (opcode==0x00E0)
    {
        screen.Clear();
    }
    else if (opcode==0x00EE)
    {
        PC = stack[SP];
        SP--;
    }
    else
    {
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
    }
}

void Chip8::ExecSet(unsigned short int opcode)
{
    unsigned char opcodeSufix = (opcode & 0x000F);
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    switch (opcodeSufix)
    {
    case 0x0:
        V[x] = V[y];
        break;
    case 0x1:
        V[x] = V[x] | V[y];
        break;
    case 0x2:
        V[x] = V[x] & V[y];
        break;
    case 0x3:
        V[x] = V[x] ^ V[y];
        break;
    case 0x4:
        V[x] = V[x] + V[y];
        V[0xF] = V[x] < V[y] ? 1 : 0;
        break;
    case 0x5:
        V[0xF] = V[x] < V[y] ? 0 : 1;
        V[x] = V[x] - V[y];
        break;
    case 0x6:
        V[0xF] = (V[x] & 0x0001) ? 1 : 0;
        V[x] = V[x] >> 1;
        break;
    case 0x7:
        V[0xF] = V[y] > V[x] ? 1 : 0;
        V[x] = V[y] - V[x];
        break;
    case 0XE:
        V[0xF] = (V[x] & 0x8000) ? 1 : 0;
        V[x] = V[x] << 1;
        break;
    default:
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
        break;
    }
}

void Chip8::WaitKeyPress(unsigned short int opcode)
{
    int keyHasBeenPressed = FALSE;
    int x = (opcode & 0x0F00) >> 8;
    while (!keyHasBeenPressed)
    {
        for (int i = 0; i < 16; i++)
        {
            if (IsKeyDown(keys[i]))
            {
                keyHasBeenPressed = TRUE;
                V[x] = i+1;
            }
        }
    }
}


void Chip8::ExecFPRefix(unsigned short int opcode)
{
    unsigned char opcodeSufix = (opcode & 0x00FF);
    int x = (opcode & 0x0F00) >> 8;
    switch (opcodeSufix)
    {
    case 0x07:
        V[x] = delayTimer;
        break;
    case 0x0A:
        WaitKeyPress(opcode);
        break;
    case 0x15:
        delayTimer = V[x];
        break;
    case 0x18:
        soundTimer = V[x];
        break;
    case 0x1E:
        I = I + V[x];
        break;
    case 0x29:
        I = 5 * V[x];
        break;
    case 0x33:
        StoreBCD();
        break;
    case 0x55:
        for (int i = 0; i < x+1; i++)
        {
            memory[I+i] = V[i];
        }
        break;
    case 0x65:
        for (int i = 0; i < x+1; i++)
        {
            V[i] = memory[I+i];
        }
        break;
    default:
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
        break;
    }
}

void Chip8::SkipIfKeyDown(unsigned short int opcode)
{
    unsigned char opcodeSufix = (opcode & 0x00FF);
    int x = (opcode & 0x0F00) >> 8;
    switch (opcodeSufix)
    {
    case 0x9E:
        PC = IsKeyDown(keys[V[x]-1]) ? PC+2 : PC;
        break;
    case 0xA1:
        PC = IsKeyDown(keys[V[x]-1]) ? PC : PC+2;
        break;
    default:
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
        break;
    }
}

void Chip8::DecodeExecute(unsigned short int opcode)
{
    unsigned char opcodePrefix = (opcode & 0xF000) >> 12;
    switch (opcodePrefix)
    {
    case 0x0:
        ExecZeroPrefix(opcode);
        break;
    case 0x1:
        PC = (opcode & 0x0FFF);
        break;
    case 0x2:
        SP++;
        stack[SP] = PC;
        PC = (opcode & 0x0FFF);
        break;
    case 0x3:
        int x = (opcode & 0x0F00) >> 8;
        unsigned char immediate = (opcode & 0x00FF);
        PC = (V[x]==immediate) ? PC+2 : PC;
        break;
    case 0x4:
        int x = (opcode & 0x0F00) >> 8;
        unsigned char immediate = (opcode & 0x00FF);
        PC = (V[x]==immediate) ? PC : PC+2;
        break;
    case 0x5:
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        PC = (V[x]==V[y]) ? PC+2 : PC;
        break;
    case 0x6:
        int x = (opcode & 0x0F00) >> 8;
        unsigned char immediate = (opcode & 0x00FF);
        V[x] = immediate;
        break;
    case 0x7:
        int x = (opcode & 0x0F00) >> 8;
        unsigned char immediate = (opcode & 0x00FF);
        V[x] += immediate;
        break;
    case 0x8:
        ExecSet(opcode);
        break;
    case 0x9:
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        PC = (V[x]!=V[y]) ? PC+2 : PC;
        break;
    case 0xA:
        unsigned short int immediate = (opcode & 0x0FFF);
        I = immediate;
        break;
    case 0xB:
        PC = (opcode & 0X0FFF) + V[0];
        break;
    case 0xC:
        unsigned char randNumber = rand() % 255;
        int x = (opcode & 0x0F00) >> 8;
        unsigned char immediate = (opcode & 0x00FF);
        V[x] = randNumber & immediate;
        break;
    case 0xD:
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        int n = (opcode & 0x000F);
        V[0xF] = screen.Draw(&V[I], V[x], V[y], n);
        break;
    case 0xE:
        SkipIfKeyDown(opcode);
        break;
    case 0xF:
        ExecFPRefix(opcode);
        break;
    default:
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
        break;
    }
}

void Chip8::Cycle()
{
    if (delayTimer>0)
    {
        delayTimer--;
    }
    if (soundTimer>0 && !audioDevice.IsPlaying())
    {   
        audioDevice.Start();
        soundTimer--;
    }
    else if (soundTimer>0)
    {
        soundTimer--;
    }
    else if (audioDevice.IsPlaying())
    {
        audioDevice.Stop();
    }
    unsigned short int opcode = Fetch();
    DecodeExecute(opcode);
}