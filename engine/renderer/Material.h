#pragma once

#include "json.hpp"
#include <fstream>
#include "assets/Asset.h"
#include "assets/AssetManager.h"
#include "Shader.h"
#include "MaterialProperties.h"
#include "core/Log.h"

namespace nimo{
 
    class Material : public Asset{
    public:
        Material(const std::shared_ptr<Shader>& shader, std::vector<IMaterialProperty*> properties)
            : shader(shader)
            , shaderBackup(shader)
            , properties(properties)
        {}
        Material(const std::string& filepath)
        {
            NIMO_DEBUG("nimo::Material::Material({})", filepath);
            std::ifstream fin(filepath);
            nlohmann::json j;
            j << fin;
            shader = AssetManager::Get<Shader>(AssetId(std::string(j["shader"])));
            shaderBackup = shader;
            for(auto o : j["properties"])
            {
                if(o["type"] == "vector2")
                {
                    auto p = new MaterialProperty<glm::vec2>();
                    p->name = o["name"];
                    p->type = ShaderUniformDataType::Float2;
                    p->SetValue({o["value"][0], o["value"][1]});
                    properties.push_back(p);
                }
                if(o["type"] == "vector3")
                {
                    auto p = new MaterialProperty<glm::vec3>();
                    p->name = o["name"];
                    p->type = ShaderUniformDataType::Float3;
                    p->SetValue({o["value"][0], o["value"][1], o["value"][2]});
                    properties.push_back(p);
                }
                if(o["type"] == "vector4")
                {
                    auto p = new MaterialProperty<glm::vec4>();
                    p->name = o["name"];
                    p->type = ShaderUniformDataType::Float4;
                    p->SetValue({o["value"][0],o["value"][1],o["value"][2], o["value"][3]});
                    properties.push_back(p);
                }
                if(o["type"] == "texture")
                {
                    auto p = new MaterialPropertyTexture();
                    p->name = o["name"];
                    p->type = ShaderUniformDataType::Sampler2D;
                    p->SetValue(o["value"]);
                    p->SetTexture(AssetManager::Get<Texture>(AssetId(std::string(o["asset"]))));
                    properties.push_back(p);
                }
            }
        }
        virtual ~Material(){
            NIMO_DEBUG("nimo::Material::~Material");
            for(auto p : properties)
                delete p;
        }
        AssetType Type() const { return AssetType::Material; }
        static AssetType StaticType(){return AssetType::Material;}
        void setShader(std::shared_ptr<Shader>& sh) { shader = sh; }
        void restoreShader() { if(shader != shaderBackup) shader = shaderBackup; }
        void Setup(){
            for(auto p : properties)
                p->Setup(shader.get());
        }
        bool HasProperty(const std::string& name, ShaderUniformDataType type)
        {
            for(auto p : properties)
                if(name == p->name && type == p->type)
                    return true;
            return false;
        }
        void ClearProperties()
        {
            for(auto p : properties)
                delete p;
            properties.clear();
        }

        void AddProperty(const std::string& name, ShaderUniformDataType type)
        {
            switch (type)
            {
            case ShaderUniformDataType::Float2 :
                {
                    auto p = new MaterialProperty<glm::vec2>();
                    p->name = name;
                    p->type = type;
                    properties.push_back(p);
                }
                break;
            case ShaderUniformDataType::Float3:
                {
                    auto p = new MaterialProperty<glm::vec3>();
                    p->name = name;
                    p->type = type;
                    properties.push_back(p);
                }
                break;
            case ShaderUniformDataType::Float4 :
                {
                    auto p = new MaterialProperty<glm::vec4>();
                    p->name = name;
                    p->type = type;
                    properties.push_back(p);
                }
                break;
            case ShaderUniformDataType::Sampler2D:
                {
                    auto p = new MaterialPropertyTexture();
                    p->name = name;
                    p->type = type;
                    properties.push_back(p);
                }
                break;
            
            default:
                break;
            }
        }
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Shader> shaderBackup;
        std::vector<IMaterialProperty*> properties;
    };
};