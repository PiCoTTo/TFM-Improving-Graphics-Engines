#include "AudioSource.h"
#include "AudioEngine.h"
#include "core/Log.h"

nimo::AudioSource::AudioSource(const std::string& filepath)
    : filepath(filepath)
{
    NIMO_DEBUG("nimo::AudioSound::AudioSource({})", filepath);
    ma_sound_init_from_file(AudioEngine::GetEngine(), filepath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_sound);
}
nimo::AudioSource::~AudioSource()
{
    NIMO_DEBUG("nimo::AudioSound::~AudioSource({})", filepath);
    ma_sound_uninit(&m_sound);
}
float nimo::AudioSource::GetDuration()
{
    float dur;
    ma_sound_get_length_in_seconds(&m_sound, &dur);
    return dur;
}

