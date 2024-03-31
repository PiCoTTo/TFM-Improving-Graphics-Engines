#include "AssetIndex.h"

nimo::AssetMetadata& nimo::AssetIndex::operator[](const AssetId id)
{
    return m_db[id];
}

const nimo::AssetMetadata& nimo::AssetIndex::get(const AssetId id) const
{
    return m_db.at(id);
}

const nimo::AssetMetadata& nimo::AssetIndex::get(const std::filesystem::path& path) const
{
    for (auto& [id, metadata] : m_db)
    {
        if (metadata.filepath == path)
            return metadata;
    }
    return AssetMetadata();
}

bool nimo::AssetIndex::contains(const AssetId id) const
{
    return m_db.find(id) != m_db.end();
}

size_t nimo::AssetIndex::remove(const AssetId id)
{
    return m_db.erase(id);
}

void nimo::AssetIndex::clear()
{
    m_db.clear();
}