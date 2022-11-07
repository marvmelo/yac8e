#include <math.h>

int GetBit(unsigned char byte, int position)
{
    unsigned char compare = (unsigned char)pow(2, position);
    byte = byte & compare;
    byte = byte >> position;
    return byte;
}