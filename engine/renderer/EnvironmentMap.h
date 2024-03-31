#pragma once

#include <string>
#include "assets/Asset.h"

namespace nimo{
class EnvironmentMap : public Asset
{
public:
    EnvironmentMap(const std::string& path, unsigned int renderResolution);
    ~EnvironmentMap();
    void Bind(unsigned int slot);
    void BindIrradiance(unsigned int slot);
    unsigned int GetInternalId() {return m_id;}

    AssetType Type() const { return AssetType::EnvironmentMap; }
    static AssetType StaticType(){return AssetType::EnvironmentMap;}

    inline unsigned int Width(){return m_width;}
    inline unsigned int Height(){return m_height;}

private:
    unsigned int m_id;
    unsigned int m_cubemap;
    unsigned int m_irradianceMap;
    unsigned int m_width;
    unsigned int m_height;
};
};