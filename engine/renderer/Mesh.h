#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <memory>
#include <vector>
#include "assets/Asset.h"
#include "glm/glm.hpp"
//#include "scene/Components.h"


namespace nimo{
    class TransformComponent;

    struct Vertex{
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };
    struct QuadVertex{
        glm::vec3 position;
        glm::vec2 uv;
    };
    struct AABB {
        glm::vec3 max{ 0,0,0 };
        glm::vec3 min{ 0,0,0 };
    };

    struct Plane
    {
        Plane() {}
        Plane(const glm::vec3& p1, const glm::vec3& norm)
            : position(p1),
            normal(glm::normalize(norm)),
            distance(glm::dot(normal, p1))
        {}

        glm::vec3 position{ 0.f, 0.f, 0.f };
        glm::vec3 normal{ 0.f, 0.f, 0.f };
        float distance{ 0.f };

        float getSignedDistanceToPlane(const glm::vec3& point) const
        {
            return glm::dot(normal, point) - distance;
        }
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;
        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;

        enum class FrustumVertice : unsigned int
        {
            FarBottomLeft,
            FarTopLeft,
            FarTopRight,
            FarBottomRight,
            NearBottomLeft,
            NearTopLeft,
            NearTopRight,
            NearBottomRight
        };

        std::vector<glm::vec3> visibleVertices;
    };

    struct BoundingVolume
    {
        virtual bool isOnFrustum(const std::shared_ptr <Frustum>& camFrustum,
            const nimo::TransformComponent& modelTransform) const = 0;
    };

    struct OOB : public BoundingVolume
    {
        glm::vec3 center{ 0.f, 0.f, 0.f };
        glm::vec3 extents{ 0.f, 0.f, 0.f };

        OOB(const glm::vec3& min, const glm::vec3& max);

        OOB(const glm::vec3& inCenter, float iI, float iJ, float iK);

        bool isOnFrustum(const std::shared_ptr<Frustum>& camFrustum, const TransformComponent& modelTransform) const override;
        bool isOnOrForwardPlane(const glm::mat4& modelMatrix, const Plane& plane) const;

    private:
        std::vector<glm::vec3> vertices;

        void initVertices(const glm::vec3& min, const glm::vec3& max);
    };

    struct Submesh{
        Submesh();
        ~Submesh();
        void Submit();
        VertexArray* m_vao = nullptr;
        VertexBuffer* m_vbo = nullptr;
        IndexBuffer* m_ibo = nullptr;
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        std::string m_name;
    };
    class Mesh : public Asset{
    public:
        Mesh(const std::string& file, bool mergeMeshesByMaterial = true);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();
        AssetType Type() const { return AssetType::Mesh; }
        static AssetType StaticType(){return AssetType::Mesh;}
        std::shared_ptr<Submesh> GetSubmesh(unsigned int id);
        const std::vector<std::shared_ptr<Submesh>>& GetSubmeshes(){ return m_submeshes;}
        glm::vec3 getCenter();
        std::shared_ptr<OOB>& getOOB();
    private:
        std::vector<std::shared_ptr<Submesh>> m_submeshes;
        std::shared_ptr<OOB> m_oob;

        AABB m_aabb;
        glm::vec3 m_center;
    };
};