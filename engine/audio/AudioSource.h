#pragma once

#include <string>
#include "assets/Asset.h"
#include "miniaudio.h"

namespace nimo{
class AudioSource : public Asset
{
public:
    AudioSource(const std::string& filepath);
    ~AudioSource();
    float GetDuration();
    inline ma_sound* GetSound() {return &m_sound;}
    std::string filepath;

    static AssetType StaticType(){return AssetType::Audio;}
private:
    ma_sound m_sound;
};
}