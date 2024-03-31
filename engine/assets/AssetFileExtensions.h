#pragma once

#include <unordered_map>
#include "AssetType.h"

namespace nimo
{
class AssetFileExtensions
{
public:
    static AssetType GetTypeFromExtension(std::string_view extension);
private:
    static std::unordered_map<std::string, AssetType> m_extensions;
};
};