#pragma once

#include <string>
#include <memory>
#include "miniaudio.h"
#include "AudioSource.h"

namespace nimo{
class AudioSound
{
public:
    AudioSound(std::shared_ptr<AudioSource> source);
    ~AudioSound();
    void SetPitch(float pitch);
    void SetVolume(float volume);
    void SetPan(float pan);
    void SetLoop(bool loop);
    bool IsPlaying();
    void Play();
    void Stop();
    void Resume();

private:
    ma_sound m_sound;
};
}