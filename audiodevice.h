#include <raylib.h>
#include <math.h>

#define MAX_SAMPLES_PER_UPDATE   4096

void AudioInputCallback(void *buffer, unsigned int frames)
{
    float sineIdx = 0.0f;
    float incr = 300.0f/44100.0f;
    short *d = (short *)buffer;

    for (int i = 0; i < frames; i++)
    {
        d[i] = (short)(32000.0f*sinf(2*PI*sineIdx));
        sineIdx += incr;
        if (sineIdx > 1.0f) sineIdx -= 1.0f;
    }
}

class AudioDevice
{
private:
    AudioStream stream;
public:
    AudioDevice(/* args */);
    void Start();
    void Stop();
    int IsPlaying();
};


AudioDevice::AudioDevice()
{
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    stream = LoadAudioStream(44100, 16, 1);
    SetAudioStreamCallback(stream, AudioInputCallback);
}

void AudioDevice::Start()
{
    PlayAudioStream(stream);
}

void AudioDevice::Stop()
{
    PauseAudioStream(stream);
}

int AudioDevice::IsPlaying()
{
    return (int)IsAudioStreamPlaying(stream);
}