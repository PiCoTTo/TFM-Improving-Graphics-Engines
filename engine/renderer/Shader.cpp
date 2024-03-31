#include "Shader.h"
#include "glad/glad.h"
#include "core/Log.h"

#include <fstream>
#include <sstream>

nimo::Shader::Shader(const std::string& filename)
{
    NIMO_DEBUG("nimo::Shader::Shader({})", filename);
    try 
    {
        std::ifstream vShaderFile;
        // open files
        vShaderFile.open(filename);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        // convert stream into string
        std::string code = vShaderStream.str();
        m_vertexCode =  code.substr(code.find("@VERTEX", 0) + 7, code.find("@VERTEXEND", 0) - code.find("@VERTEX", 0) - 7);
        m_fragmentCode = code.substr(code.find("@FRAGMENT", 0) + 9, code.find("@FRAGMENTEND", 0) - code.find("@FRAGMENT", 0) - 9);
        m_usable = Compile(m_vertexCode, m_fragmentCode);
    }
    catch (std::ifstream::failure& e)
    {
        NIMO_ERROR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {}", filename);
    }
}
bool nimo::Shader::Compile(const std::string& vertexCode, const std::string& fragmentCode)
{
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if(!CheckCompileErrors(vertex, "VERTEX"))
    {
        glDeleteShader(vertex);
        return false;
    }
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if(!CheckCompileErrors(fragment, "FRAGMENT"))
    {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    // shader Program
    auto tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);
    if(!CheckCompileErrors(tempID, "PROGRAM"))
    {
        glDeleteProgram(tempID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_ID = tempID;

    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 256; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &count);
    NIMO_DEBUG("Active Attributes: {}", count);

    for (int i = 0; i < count; i++)
    {
        glGetActiveAttrib(m_ID, (GLuint)i, bufSize, &length, &size, &type, name);
        NIMO_DEBUG("Attribute #{} Type: 0x{:0x} Name: {}", i, type, name);
    }

    glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &count);
    NIMO_DEBUG("Active Uniforms: {}", count);
    m_uniforms.clear();
    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(m_ID, (GLuint)i, bufSize, &length, &size, &type, name);
        NIMO_DEBUG("Uniform #{} Type: 0x{:0x} Name: {} Size: {}", i, type, name, size);
        switch (type)
        {
        case GL_FLOAT:
            m_uniforms.push_back({name, ShaderUniformDataType::Float});
            break;
        case GL_FLOAT_VEC2:
            m_uniforms.push_back({name, ShaderUniformDataType::Float2});
            break;
        case GL_FLOAT_VEC3:
            m_uniforms.push_back({name, ShaderUniformDataType::Float3});
            break;
        case GL_FLOAT_VEC4:
            m_uniforms.push_back({name, ShaderUniformDataType::Float4});
            break;
        case GL_INT:
            m_uniforms.push_back({name, ShaderUniformDataType::Int});
            break;
        case GL_INT_VEC2:
            m_uniforms.push_back({name, ShaderUniformDataType::Int2});
            break;
        case GL_INT_VEC3:
            m_uniforms.push_back({name, ShaderUniformDataType::Int3});
            break;
        case GL_INT_VEC4:
            m_uniforms.push_back({name, ShaderUniformDataType::Int4});
            break;
        case GL_BOOL:
            m_uniforms.push_back({name, ShaderUniformDataType::Bool});
            break;
        case GL_FLOAT_MAT2:
            m_uniforms.push_back({name, ShaderUniformDataType::Mat2});
            break;
        case GL_FLOAT_MAT3:
            m_uniforms.push_back({name, ShaderUniformDataType::Mat3});
            break;
        case GL_FLOAT_MAT4:
            m_uniforms.push_back({name, ShaderUniformDataType::Mat4});
            break;
        case GL_SAMPLER_1D:
            m_uniforms.push_back({name, ShaderUniformDataType::Sampler1D});
            break;
        case GL_SAMPLER_2D:
            m_uniforms.push_back({name, ShaderUniformDataType::Sampler2D});
            break;
        case GL_SAMPLER_3D:
            m_uniforms.push_back({name, ShaderUniformDataType::Sampler3D});
            break;
        case GL_SAMPLER_CUBE:
            m_uniforms.push_back({name, ShaderUniformDataType::SamplerCube});
            break;
        default:
            NIMO_ERROR("Unsupported uniform type: 0x{:0x} for ",type, name);
            break;
        }
    }
    return true;
}
nimo::Shader::~Shader()
{
    NIMO_DEBUG("nimo::Shader::~Shader");
    glDeleteProgram(m_ID);
}
void nimo::Shader::use()
{
    if(m_usable)
        glUseProgram(m_ID);
}
void nimo::Shader::Set(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}
void nimo::Shader::Set(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); 
}
void nimo::Shader::Set(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); 
}
void nimo::Shader::Set(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
}
void nimo::Shader::Set(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
}
void nimo::Shader::Set(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
}
void nimo::Shader::Set(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z); 
}
void nimo::Shader::Set(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
}
void nimo::Shader::Set(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w); 
}
void nimo::Shader::Set(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void nimo::Shader::Set(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void nimo::Shader::Set(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
bool nimo::Shader::CheckCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
            NIMO_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {}\n {}", type , &errorLog[0]);
            return false;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
            NIMO_ERROR("ERROR::PROGRAM_LINKING_ERROR of type: {}\n {}", type , &errorLog[0]);
            return false;
        }
    }
    return true;
}