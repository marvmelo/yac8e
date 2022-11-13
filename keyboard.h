#include <raylib.h>
#include <iostream>
#define TRUE 1
#define FALSE 0

using namespace std;

void CheckKeys(int keysPressed[16])
{
    KeyboardKey keysTemp[16] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
                                KEY_Q, KEY_W, KEY_E, KEY_A,
                                KEY_S, KEY_D, KEY_Z, KEY_C,
                                KEY_FOUR, KEY_R, KEY_F, KEY_V};

    for (int i = 0; i < 16; i++)
    {
        if (IsKeyDown(keysTemp[i]))
        {
            keysPressed[i] = TRUE;
        }
        else
        {
        keysPressed[i] = FALSE;
        }
    }
}