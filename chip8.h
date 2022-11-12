#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <math.h>
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

    unsigned short int Fetch();
    void DecodeExecute(unsigned short int);
    void ExecZeroPrefix(unsigned short int);
    void ExecSet(unsigned short int);
    void SkipIfKeyDown(unsigned short int);
    void ExecFPRefix(unsigned short int);
    void WaitKeyPress(unsigned short int);
    void StoreBCD(int);

public:
    Chip8(char *, int);
    void Cycle();
};

Chip8::Chip8(char *ROMArray, int size)
{
    int ROMsize = fmin(size, 3584);
    for (int i = 0; i < ROMsize; i++)
    {
        memory[0x200+i] = (unsigned char)ROMArray[i];
    }
    for (int i = 0; i < 16; i++)
    {
        V[i] = 0;
    }
    delayTimer = 0;
    soundTimer = 0;
    I = 0;
    PC = 0x200;
    SP = -1;
    for (int i = 0; i < 16; i++)
    {
        stack[i] = 0;
    }
    screen = Screen();
    audioDevice =AudioDevice();
    KeyboardKey keysTemp[16] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
                                KEY_Q, KEY_W, KEY_E, KEY_A,
                                KEY_S, KEY_D, KEY_Z, KEY_C,
                                KEY_FOUR, KEY_R, KEY_F, KEY_V};
    for (int i = 0; i < 16; i++)
    {
        keys[i] = keysTemp[i];
    }
    
    unsigned char fontSet[80]  =
        {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        for (int i = 0; i < 80; i++)
        {
            memory[i] = fontSet[i];
        }
        
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

void Chip8::StoreBCD(int x)
{
    unsigned char hundreds = V[x] / 100;
    unsigned char tens = (V[x] - 100*hundreds) / 10;
    unsigned char ones = (V[x] - 100*hundreds - 10*tens);
    memory[I] = hundreds;
    memory[I+1] = tens;
    memory[I+2] = ones;
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
        V[0xF] = (V[x] & 0x0080) ? 1 : 0;
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
            if (IsKeyPressed(keys[i]))
            {
                keyHasBeenPressed = TRUE;
                V[x] = i;
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
        StoreBCD(x);
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
        PC = IsKeyPressed(keys[V[x]]) ? PC+2 : PC;
        break;
    case 0xA1:
        PC = IsKeyPressed(keys[V[x]]) ? PC : PC+2;
        break;
    default:
        cout << "Opcode " << setfill('0') << setw(4) << hex << opcode << "not recognized.";
        break;
    }
}

void Chip8::DecodeExecute(unsigned short int opcode)
{
    unsigned char opcodePrefix = (opcode & 0xF000) >> 12;
    int x;
    int y;
    unsigned char immediate8;
    unsigned char randNumber;
    unsigned short int immediate16;
    int n;
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
        x = (opcode & 0x0F00) >> 8;
        immediate8 = (opcode & 0x00FF);
        PC = (V[x]==immediate8) ? PC+2 : PC;
        break;
    case 0x4:
        x = (opcode & 0x0F00) >> 8;
        immediate8 = (opcode & 0x00FF);
        PC = (V[x]==immediate8) ? PC : PC+2;
        break;
    case 0x5:
        x = (opcode & 0x0F00) >> 8;
        y = (opcode & 0x00F0) >> 4;
        PC = (V[x]==V[y]) ? PC+2 : PC;
        break;
    case 0x6:
        x = (opcode & 0x0F00) >> 8;
        immediate8 = (opcode & 0x00FF);
        V[x] = immediate8;
        break;
    case 0x7:
        x = (opcode & 0x0F00) >> 8;
        immediate8 = (opcode & 0x00FF);
        V[x] += immediate8;
        break;
    case 0x8:
        ExecSet(opcode);
        break;
    case 0x9:
        x = (opcode & 0x0F00) >> 8;
        y = (opcode & 0x00F0) >> 4;
        PC = (V[x]!=V[y]) ? PC+2 : PC;
        break;
    case 0xA:
        immediate16 = (opcode & 0x0FFF);
        I = immediate16;
        break;
    case 0xB:
        PC = (opcode & 0X0FFF) + V[0];
        break;
    case 0xC:
        randNumber = rand() % 255;
        x = (opcode & 0x0F00) >> 8;
        immediate8 = (opcode & 0x00FF);
        V[x] = randNumber & immediate8;
        break;
    case 0xD:
        x = (opcode & 0x0F00) >> 8;
        y = (opcode & 0x00F0) >> 4;
        n = (opcode & 0x000F);
        V[0xF] = screen.Draw(&memory[I], V[x], V[y], n);
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