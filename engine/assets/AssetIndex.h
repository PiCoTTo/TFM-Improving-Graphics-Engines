#pragma once

#include <unordered_map>

#include "AssetMetadata.h"

namespace nimo
{
class AssetIndex
{
public:
    AssetMetadata& operator[](const AssetId handle);
    const AssetMetadata& get(const AssetId handle) const;
    const AssetMetadata& get(const std::filesystem::path& path) const;

    size_t count() const { return m_db.size(); }
    bool contains(const AssetId handle) const;
    size_t remove(const AssetId handle);
    void clear();

    auto begin() { return m_db.begin(); }
    auto end() { return m_db.end(); }
    auto begin() const { return m_db.cbegin(); }
    auto end() const { return m_db.cend(); }
private:
    std::unordered_map<AssetId, AssetMetadata> m_db;
};
} // namespace nimo