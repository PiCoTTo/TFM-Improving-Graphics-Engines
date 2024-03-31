#pragma once
#include "Shader.h"
#include "Texture.h"

namespace nimo
{
    class IMaterialProperty{
    public:
        virtual ~IMaterialProperty(){}
        std::string name;
        ShaderUniformDataType type;
        virtual void Setup(Shader* shader){}
        virtual void* GetDataPtr() = 0;
    };
    template<typename T>
    class MaterialProperty : public IMaterialProperty{
    public:
        void Setup(Shader* shader) override {
            shader->Set(name, value);
        }
        void* GetDataPtr() override {
            return (void*)&value;
        }
        void SetValue(const T& v){
            value = v;
        }
    private:
        T value;
    };
    class MaterialPropertyTexture : public IMaterialProperty{
    public:
        MaterialPropertyTexture() {type = ShaderUniformDataType::Sampler2D;}
        void Setup(Shader* shader) override {
            if (t) {
                shader->Set(name, value);
                t->bind(value);
            }
        }
        void* GetDataPtr() override {
            return (void*)&value;
        }
        void SetValue(int v){
            value = v;
        }
        int GetValue(){return value;}
        void SetTexture(std::shared_ptr<Texture> v){
            t = v;
        }
        std::shared_ptr<Texture> GetTexture(){return t;}
    private:
        int value;
        std::shared_ptr<Texture> t;
    };
} // namespace nimo
