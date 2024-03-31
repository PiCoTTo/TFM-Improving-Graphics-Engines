#include "AudioEngine.h"
#include "core/Log.h"
ma_engine* nimo::AudioEngine::m_engine = nullptr;
void nimo::AudioEngine::Initialize()
{
    ma_result result;
    m_engine = (ma_engine*)malloc(sizeof(ma_engine));
    result = ma_engine_init(NULL, m_engine);
    if (result != MA_SUCCESS) {
        NIMO_CRITICAL("Failed to initialize audio engine.");
    }
    ma_engine_set_volume(m_engine, 0.1f);
}
void nimo::AudioEngine::Cleanup()
{
    ma_engine_uninit(m_engine);
    free(m_engine);
}