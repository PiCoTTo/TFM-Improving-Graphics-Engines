#pragma once

#include "Scene.h"
#include <memory>
#include <filesystem>
#include "assets/AssetSerializer.h"
#include "json.hpp"

namespace nimo {
    template<> struct AssetSerializer<Scene>{
        void Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Scene>& asset);
        std::shared_ptr<nimo::Scene> Deserialize(const nimo::AssetMetadata& metadata);
		nlohmann::ordered_json SerializeEntity(const std::shared_ptr<nimo::Scene>& scene, const Entity& entity);
		nimo::GUID DeserializeEntity(const std::shared_ptr<nimo::Scene>& scene, const nlohmann::ordered_json& source, GUID desiredId = {}, GUID parentId = {});
    };
}
