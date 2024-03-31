#pragma once
#include <string>
#include <memory>
#include "assets/Asset.h"
#include "assets/AssetType.h"

namespace nimo
{
enum class ScriptFieldType{
    Number,
    String,
    Boolean,
    Asset,
    Entity,
};
struct ScriptField
{
    ScriptField(const std::string name) : name(name){}
    virtual ~ScriptField(){}
    virtual ScriptFieldType GetType() = 0;
    std::string name;
};
struct ScriptFieldNumber : public ScriptField
{
    ScriptFieldNumber(const std::string& name, double value) 
        : ScriptField(name), value(value) {}
    ScriptFieldType GetType() { return ScriptFieldType::Number;};
    float value;
};
struct ScriptFieldString : public ScriptField
{
    ScriptFieldString(const std::string& name, const std::string& value) 
        : ScriptField(name), value(value) {}
    ScriptFieldType GetType() { return ScriptFieldType::String;};
    std::string value;
};
struct ScriptFieldBool : public ScriptField
{
    ScriptFieldBool(const std::string& name, bool value) 
        : ScriptField(name), value(value) {}
    ScriptFieldType GetType() { return ScriptFieldType::Boolean;};
    bool value;
};
struct ScriptFieldAsset : public ScriptField
{
    ScriptFieldAsset(const std::string& name, AssetType type) 
        : ScriptField(name), value({}), type(type) {}
    ScriptFieldAsset(const std::string& name, std::shared_ptr<Asset> value, AssetType type) 
        : ScriptField(name), value(value), type(type){}
    ScriptFieldType GetType() { return ScriptFieldType::Asset;};
    std::shared_ptr<Asset> value;
    AssetType type;
};
struct ScriptFieldEntity : public ScriptField
{
    ScriptFieldEntity(const std::string& name) 
        : ScriptField(name), entity(){}
    ScriptFieldEntity(const std::string& name, GUID entity) 
        : ScriptField(name), entity(entity){}
    ScriptFieldType GetType() { return ScriptFieldType::Entity;};
    GUID entity;
};
} // namespace nimo
