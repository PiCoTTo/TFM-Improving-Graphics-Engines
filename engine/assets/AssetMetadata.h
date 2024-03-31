#pragma once

#include <filesystem>

#include "Asset.h"
#include "AssetSettings.h"

namespace nimo
{
class AssetMetadata
{
public:
    AssetId id;
    AssetType type;
    std::filesystem::path filepath;
    std::shared_ptr<IAssetSettings> serializerSettings;
};
} // namespace nimo

