#pragma once

#include <string>
#include "assets/Asset.h"

namespace nimo{
class Script : public Asset
{
public:
    Script(const std::string& filepath);
    std::string m_filepath;

    static AssetType StaticType(){return AssetType::Script;}
private:
};
}