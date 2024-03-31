#pragma once 
#include "core/GUID.h"
#include "AssetType.h"

namespace nimo
{
using AssetId = GUID;
class Asset
{
public:
    virtual ~Asset(){}
    AssetId id;
    static AssetType StaticType() { return AssetType::None; }
    virtual AssetType Type() const { return AssetType::None; }
    virtual bool operator==(const Asset& other) const
    {
        return id == other.id;
    }
    
    virtual bool operator!=(const Asset& other) const
    {
        return !(*this == other);
    }

};
};