#include "AudioSound.h"
#include "AudioEngine.h"
#include "core/Log.h"

nimo::AudioSound::AudioSound(std::shared_ptr<AudioSource> source)
{
    NIMO_DEBUG("nimo::AudioSound::AudioSound({})", source->id.Str());
    ma_sound_init_copy(AudioEngine::GetEngine(), source->GetSound(), MA_SOUND_FLAG_DECODE, NULL, &m_sound);
    // ma_sound_is_looping(&sound);
    // ma_sound_set_fade_in_milliseconds(&sound, 0, 1, 1000);
    // ma_sound_set_fade_in_milliseconds(&sound, -1, 0, 1000);
}
nimo::AudioSound::~AudioSound()
{
    NIMO_DEBUG("nimo::AudioSound::~AudioSound()");
    ma_sound_uninit(&m_sound);
}
void nimo::AudioSound::SetPitch(float pitch)
{
    ma_sound_set_pitch(&m_sound, pitch);
}
void nimo::AudioSound::SetVolume(float volume)
{
    ma_sound_set_volume(&m_sound, volume);
}
void nimo::AudioSound::SetPan(float pan)
{
    ma_sound_set_pan(&m_sound, pan);
}
void nimo::AudioSound::SetLoop(bool loop)
{
    ma_sound_set_looping(&m_sound, loop);
}
void nimo::AudioSound::Play()
{
    ma_sound_seek_to_pcm_frame(&m_sound, 0);
    ma_sound_start(&m_sound);
}
void nimo::AudioSound::Stop()
{
    ma_sound_stop(&m_sound);
}
void nimo::AudioSound::Resume()
{
    ma_sound_start(&m_sound);
}
bool nimo::AudioSound::IsPlaying()
{
    return ma_sound_is_playing(&m_sound);
}
