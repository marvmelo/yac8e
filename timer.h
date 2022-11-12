#include <raylib.h>

using namespace std;

void WaitCycle(double startTime, double period)
{
    double currentTime = GetTime();
    while ((currentTime -startTime)<period)
    {
        currentTime = GetTime();
    }
}