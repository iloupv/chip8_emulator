#include "PlatformSound.hpp"
#include <cmath>
#include <iostream>

PlatformSound::PlatformSound()
    : m_sineFreq(1000),
      m_sampleFreq(44100),
      m_samplesPerSine(m_sampleFreq / m_sineFreq),
      m_samplePos(0)
{
    SDL_AudioSpec wantSpec, haveSpec;

    SDL_zero(wantSpec);
    wantSpec.freq = m_sampleFreq;
    wantSpec.format = AUDIO_U8;
    wantSpec.channels = 1;
    //wantSpec.samples = 2048;
    wantSpec.samples = 128;
    wantSpec.callback = SDLAudioCallback;
    wantSpec.userdata = this;

    m_device = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (m_device == 0)
    {
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
}

PlatformSound::~PlatformSound()
{
    SDL_CloseAudioDevice(m_device);
}

void PlatformSound::play()
{
    SDL_PauseAudioDevice(m_device, 0);
}

void PlatformSound::stop()
{
    SDL_PauseAudioDevice(m_device, 1);
}

void PlatformSound::SDLAudioCallback(void *data, Uint8 *buffer, int length)
{
    PlatformSound *sound = reinterpret_cast<PlatformSound*>(data);

    for(int i = 0; i < length; ++i)
    {
        buffer[i] = (std::sin(sound->m_samplePos / sound->m_samplesPerSine * M_PI * 2) + 1) * 127.5;
        ++sound->m_samplePos;
    }
}
