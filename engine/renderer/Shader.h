#pragma once
#include <string>
#include <glm/glm.hpp>
#include "assets/Asset.h"

namespace nimo{
enum class ShaderUniformDataType : unsigned int{
// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool,
    Mat2,
    Mat3,
    Mat4,
    Sampler1D,
    Sampler2D,
    Sampler3D,
    SamplerCube,
};
struct ShaderUniform{
    std::string name;
    ShaderUniformDataType type;
};
class Shader : public Asset{
public:
    Shader(const std::string& filename);
    ~Shader();
    AssetType Type() const { return AssetType::Shader; }
    static AssetType StaticType(){return AssetType::Shader;}
    void use();
    void Set(const std::string &name, bool value) const;
    void Set(const std::string &name, int value) const;
    void Set(const std::string &name, float value) const;
    void Set(const std::string &name, const glm::vec2 &value) const;
    void Set(const std::string &name, float x, float y) const;
    void Set(const std::string &name, const glm::vec3 &value) const;
    void Set(const std::string &name, float x, float y, float z) const;
    void Set(const std::string &name, const glm::vec4 &value) const;
    void Set(const std::string &name, float x, float y, float z, float w) const;
    void Set(const std::string &name, const glm::mat2 &mat) const;
    void Set(const std::string &name, const glm::mat3 &mat) const;
    void Set(const std::string &name, const glm::mat4 &mat) const;
    
    inline const std::vector<ShaderUniform>& GetUniforms() { return m_uniforms;}
    inline std::string& GetVertexCode(){ return m_vertexCode;}
    inline std::string& GetFragmentCode(){ return m_fragmentCode;}
    inline std::string* GetVertexCodePtr(){ return &m_vertexCode;}
    inline std::string* GetFragmentCodePtr(){ return &m_fragmentCode;}
    void Recompile(){Compile(m_vertexCode, m_fragmentCode);}

private:
    unsigned int m_ID;
    bool CheckCompileErrors(unsigned int shader, const std::string& type);
    bool Compile(const std::string& vertexCode, const std::string& fragmentCode);
    std::vector<ShaderUniform> m_uniforms;
    std::string m_vertexCode;
    std::string m_fragmentCode;
    bool m_usable = false;
};
};