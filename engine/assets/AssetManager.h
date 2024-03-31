#pragma once

#include "AssetIndex.h"
#include "AssetSerializer.h"
#include "core/Log.h"
#include <memory>
#include <typeindex>
#include <typeinfo>

namespace nimo
{
class AssetManager
{
public:
	static void Initialize();
	static void Cleanup();
	
	template<typename T>
	static void CreateNewAsset(const std::string& filename)
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		AssetMetadata metadata = m_index.get(filename);
		if(!metadata.id.Valid()){
			metadata.id = GUID::Create();
			metadata.filepath = filename;
			metadata.type = T::StaticType();
    		metadata.serializerSettings = CreateAssetSettings(T::StaticType());
			m_index[metadata.id] = metadata;
		}
		AssetSerializer<T> assetSerializer;
		std::shared_ptr<T> asset = assetSerializer.Deserialize(metadata);
		asset->id = metadata.id;
		m_loadedAssets[typeid(T)][asset->id] = asset;
		NIMO_INFO("Created and loaded asset {} with GUID: {}", filename, asset->id.str());
	}

	template<typename T>
	static void CreateAssetFromMemory(const std::string& filename, const std::shared_ptr<T>& asset)
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		AssetMetadata metadata = m_index.get(filename);
		if(!metadata.id.Valid()){
			metadata.id = GUID::Create();
			metadata.filepath = filename;
			metadata.type = T::StaticType();
    		metadata.serializerSettings = CreateAssetSettings(T::StaticType());
			m_index[metadata.id] = metadata;
		}
		AssetSerializer<T> assetSerializer;
		assetSerializer.Serialize(metadata, asset);
		asset->id = metadata.id;
		m_loadedAssets[typeid(T)][asset->id] = asset;
	}
	
	template <typename T>
	static void Serialize(AssetId id)
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		AssetMetadata metadata = m_index.get(id);
		NIMO_INFO("Serializing {} {}", AssetTypeToString(metadata.type), metadata.filepath.string());
		AssetSerializer<T> assetSerializer;
		assetSerializer.Serialize(metadata, std::static_pointer_cast<T, Asset>(m_loadedAssets[typeid(T)][id]));
	}

	template <typename T>
	static std::shared_ptr<T> Get(AssetId id)
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		auto it = m_loadedAssets[typeid(T)].find(id);
		if(it != m_loadedAssets[typeid(T)].end())
		{
			// Loaded
			return std::static_pointer_cast<T, Asset>(m_loadedAssets[typeid(T)][id]);
		}
		// Not loaded
		NIMO_WARN("NOT found loaded asset {}", id.Str());
		AssetSerializer<T> assetSerializer;
		std::shared_ptr<T> asset = assetSerializer.Deserialize(m_index[id]);
		asset->id = m_index[id].id;
		m_loadedAssets[typeid(T)][m_index[id].id] = asset;
		return asset;
	}

	template<typename T>
	static std::shared_ptr<T> Get(const std::string& filepath)
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		if(m_index.get(std::filesystem::path(filepath)).id.Valid())
			return Get<T>(m_index.get(std::filesystem::path(filepath)).id);
		else
			return nullptr;
	}

	template<typename T>
	static std::vector<std::shared_ptr<T>> GetAllLoaded()
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		std::vector<std::shared_ptr<T>> res;
		for(auto [id, asset] : m_loadedAssets[typeid(T)])
		{
			if(asset)
				res.push_back(std::static_pointer_cast<T, Asset>(asset));
		}
		return res;
	}

	template<typename T>
	static std::vector<AssetMetadata> GetAllExisting()
	{
		static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset class");
		std::vector<AssetMetadata> res;
		for(auto [id, metadata] : m_index)
		{
			if(metadata.type == T::StaticType())
				res.push_back(metadata);
		}
		return res;
	}

	static const AssetMetadata& GetMetadata(const std::filesystem::path& path);
	static const AssetMetadata& GetMetadata(AssetId id);
	static const AssetMetadata& GetMetadata(const std::shared_ptr<Asset>& asset) { return GetMetadata(asset->id); }

	static void LoadAssetIndex(const std::string& filename);
	static void WriteIndex();
	static std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
	static void UpdatePath(AssetId id, const std::filesystem::path& newPath);
	static void Unregister(AssetId id);
	static AssetId Import(const std::filesystem::path& filepath);
	static void ImportDirectory(const std::filesystem::path& folderpath, bool includeSubdirectories = true);

	static void UnloadUnused();

private:
	static AssetMetadata& GetMetadataRef(AssetId handle);
	static std::shared_ptr<IAssetSettings> CreateAssetSettings(AssetType type);

	static AssetIndex m_index;
	static std::unordered_map<std::type_index, std::unordered_map<AssetId, std::shared_ptr<Asset>>> m_loadedAssets;
};
} // namespace nimo
