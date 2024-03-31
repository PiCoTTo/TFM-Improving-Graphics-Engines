#pragma once

#pragma once

#include <string>
#include "assets/Asset.h"
#include "scene/Scene.h"
#include "json.hpp"

namespace nimo{
class Prefab : public Asset
{
public:
    Prefab();
    Prefab(const std::string& filepath);
    std::string filepath;
    GUID Create(std::shared_ptr<nimo::Scene> where);
    void SetData(std::shared_ptr<nimo::Scene> where, Entity e);

    static AssetType StaticType(){return AssetType::Prefab;}
    nlohmann::json m_source;
};
}