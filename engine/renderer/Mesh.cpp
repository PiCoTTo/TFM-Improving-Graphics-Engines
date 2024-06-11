#include "Mesh.h"
#include "glad/glad.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"      
#include "assimp/postprocess.h"
#include "core/Log.h"
#include "scene/Components.h"


nimo::Mesh::Mesh(const std::string& file, bool mergeMeshesByMaterial)
{
    NIMO_DEBUG("nimo::Mesh::Mesh({})", file);
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    unsigned int flags = aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate |
    aiProcess_GenSmoothNormals |
    aiProcess_CalcTangentSpace |
    aiProcess_LimitBoneWeights |
    aiProcess_ImproveCacheLocality |
    aiProcess_RemoveRedundantMaterials |
    aiProcess_GenUVCoords |
    aiProcess_SortByPType |
    aiProcess_FindDegenerates |
    aiProcess_FindInvalidData |
    aiProcess_FindInstances |
    aiProcess_ValidateDataStructure |
    aiProcess_OptimizeMeshes |
    aiProcess_Debone;
    if(mergeMeshesByMaterial)
        flags |= aiProcess_OptimizeGraph;
    const aiScene* scene = importer.ReadFile( file, flags);
    
    // If the import failed, report it
    if( !scene)
    {
        // std::cout << "Error loading assimp scene for " << file << std::endl;
    }
    else
    {
        NIMO_DEBUG("Succesfully loaded assimp scene file: {}", file);
        // std::cout << "Succesfully loaded assimp scene for " << file << std::endl;
        // std::cout << file << std::endl;
        // std::cout << "\t NumMeshes: " << scene->mNumMeshes << std::endl;
        NIMO_DEBUG("\t NumMeshes: {}", scene->mNumMeshes);
        glm::vec3 min(0,0,0), max(0,0,0);
        for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            NIMO_DEBUG("\t\t Mesh: {}", scene->mMeshes[i]->mName.C_Str());
            
            // std::cout << "\t\tMesh " << i << std::endl;
            // std::cout << "\t\t NumVertices: " << scene->mMeshes[i]->mNumVertices << std::endl;
            // std::cout << "\t\t NumFaces: " << scene->mMeshes[i]->mNumFaces << std::endl;
            // std::cout << "\t\t NumAnimMeshes: " << scene->mMeshes[i]->mNumAnimMeshes << std::endl;
            // std::cout << "\t\t NumBones: " << scene->mMeshes[i]->mNumBones << std::endl;
            auto submesh = std::make_shared<Submesh>();
            submesh->m_name = scene->mMeshes[i]->mName.C_Str();
            for(int j = 0; j< scene->mMeshes[i]->mNumVertices; ++j)
            {
                Vertex vertex;
                vertex.position = {scene->mMeshes[i]->mVertices[j].x,scene->mMeshes[i]->mVertices[j].y,scene->mMeshes[i]->mVertices[j].z};
                
                // Get AABB
                if (max.x < vertex.position.x) max.x = vertex.position.x;
                if (max.y < vertex.position.y) max.y = vertex.position.y;
                if (max.z < vertex.position.z) max.z = vertex.position.z;
                if (min.x > vertex.position.x) min.x = vertex.position.x;
                if (min.y > vertex.position.y) min.y = vertex.position.y;
                if (min.z > vertex.position.z) min.z = vertex.position.z;

                if (scene->mMeshes[i]->HasNormals())
                    vertex.normal = {scene->mMeshes[i]->mNormals[j].x,scene->mMeshes[i]->mNormals[j].y,scene->mMeshes[i]->mNormals[j].z};
                else
                {
                    NIMO_ERROR("Model has no normals");
                    vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                if(scene->mMeshes[i]->HasTangentsAndBitangents())
                {
                    vertex.tangent = {scene->mMeshes[i]->mTangents[j].x,scene->mMeshes[i]->mTangents[j].y,scene->mMeshes[i]->mTangents[j].z};
                    vertex.bitangent = {scene->mMeshes[i]->mBitangents[j].x,scene->mMeshes[i]->mBitangents[j].y,scene->mMeshes[i]->mBitangents[j].z};
                }
                else{
                    NIMO_ERROR("Model has no tangent space");
                    vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                    vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                // texture coordinates
                if (scene->mMeshes[i]->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    // std::cout << "Has uvs" << std::endl;
                    glm::vec2 vec;
                    vec.x = scene->mMeshes[i]->mTextureCoords[0][j].x;
                    vec.y = scene->mMeshes[i]->mTextureCoords[0][j].y;
                    vertex.uv = vec;
                }
                else
                    vertex.uv = glm::vec2(0.0f, 0.0f);
                // std::cout << "Vertex num " << i << "[" << vertex.position.x << ", "<< vertex.position.y << ", "<< vertex.position.z << "]["<< vertex.uv.x << ", "<< vertex.uv.y << "]"<< std::endl;
                submesh->m_vertices.push_back(vertex);
            }
            for(int j = 0; j< scene->mMeshes[i]->mNumFaces; ++j)
            {
                aiFace face = scene->mMeshes[i]->mFaces[j];
                // retrieve all indices of the face and store them in the indices vector
                for (unsigned int k = 0; k < face.mNumIndices; k++)
                    submesh->m_indices.push_back(face.mIndices[k]);
            }
            submesh->Submit();
            m_submeshes.push_back(submesh);
            m_oob = std::make_shared<OOB>(min, max);
            m_center = (max - min) * 0.5f;
        }
        // std::cout << "\t NumTextures: " << scene->mNumTextures << std::endl;
        // std::cout << "\t NumMaterials: " << scene->mNumMaterials << std::endl;
        for(unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            // std::cout << "\t\tMaterial " << i << std::endl;
            // std::cout << "\t\t NumProperties: " << scene->mMaterials[i]->mNumProperties << std::endl;
            for(unsigned int j = 0; j < scene->mMaterials[i]->mNumProperties; ++j)
            {
                // std::cout << "\t\t\tProperty " << i << std::endl;
                // std::cout << "\t\t\t Key: " << scene->mMaterials[i]->mProperties[j]->mKey.C_Str() << std::endl;
            }
        }
        // std::cout << "\t NumAnimations: " << scene->mNumAnimations << std::endl;
        // std::cout << "\t NumCameras: " << scene->mNumCameras << std::endl;
        // std::cout << "\t NumLights: " << scene->mNumLights << std::endl;

        // std::cout << "\tRootNode name: " << scene->mRootNode->mName.C_Str() << std::endl;
        // std::cout << "\tRootNode NumMeshes: " << scene->mRootNode->mNumMeshes << std::endl;
        // std::cout << "\tRootNode NumChildren: " << scene->mRootNode->mNumChildren << std::endl;
        
        NIMO_DEBUG("\tRootNode - Name: {} - NumMeshes: {}", scene->mRootNode->mName.C_Str(), scene->mRootNode->mNumMeshes);
        for(unsigned int i = 0; i < scene->mRootNode->mNumChildren; ++i)
        {
            NIMO_DEBUG("\t\tRootNode Child {} - Name: {} - NumMeshes: {}", i, scene->mRootNode->mChildren[i]->mName.C_Str(), scene->mRootNode->mChildren[i]->mNumMeshes);
            for(int j = 0; j < scene->mRootNode->mChildren[i]->mNumMeshes; ++j)
            {
                // std::cout << "\t\t\tRootNode Child Mesh Id " << scene->mRootNode->mChildren[i]->mMeshes[j] << std::endl;
            }
        }
        
    }
}
nimo::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    NIMO_DEBUG("nimo::Mesh::Mesh");
    auto submesh = std::make_shared<Submesh>();
    m_submeshes.push_back(submesh);
    submesh->m_vertices = vertices;
    submesh->m_indices = indices;
    submesh->Submit();
}
nimo::Mesh::~Mesh()
{
    NIMO_DEBUG("nimo::Mesh::~Mesh");
}

std::shared_ptr<nimo::Submesh> nimo::Mesh::GetSubmesh(unsigned int id)
{
    if(id >= m_submeshes.size())
        return m_submeshes[0];
    return  m_submeshes[id];
}

glm::vec3 nimo::Mesh::getCenter()
{
    return m_center;
}

std::shared_ptr<nimo::OOB>& nimo::Mesh::getOOB()
{
    return m_oob;
}


nimo::Submesh::Submesh()
{
    NIMO_DEBUG("nimo::Submesh::Submesh");
}
nimo::Submesh::~Submesh()
{
    NIMO_DEBUG("nimo::Submesh::~Submesh");
    if(m_vao) delete m_vao;
    if(m_ibo) delete m_ibo;
    if(m_vbo) delete m_vbo;
}
void nimo::Submesh::Submit()
{
    m_vao = new VertexArray();
    m_vbo = new VertexBuffer(
        {
            {"position", ShaderDataType::Float3},
            {"normal", ShaderDataType::Float3},
            {"uv", ShaderDataType::Float2},
            {"tangent", ShaderDataType::Float3},
            {"bitangent", ShaderDataType::Float3},
        },
        m_vertices.data(), sizeof(Vertex) * m_vertices.size()
    );
    m_ibo = new IndexBuffer(m_indices.data(), m_indices.size());
    m_vao->Bind();
    m_ibo->Bind();
    m_vbo->Bind();
    m_vbo->ApplyLayout();
}

nimo::OOB::OOB(const glm::vec3& min, const glm::vec3& max)
    : BoundingVolume{},
    center{ (max + min) * 0.5f },
    extents{ max.x - center.x, max.y - center.y, max.z - center.z }
{
    initVertices(min, max);
}

nimo::OOB::OOB(const glm::vec3& inCenter, float iI, float iJ, float iK)
    : BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
{
    initVertices(center - extents, center + extents);
}

bool nimo::OOB::isOnFrustum(const std::shared_ptr<nimo::Frustum>& camFrustum, const TransformComponent& modelTransform) const
{
    auto modelMatrix = modelTransform.GetTransform();

    return (isOnOrForwardPlane(modelMatrix, camFrustum->leftFace) &&
            isOnOrForwardPlane(modelMatrix, camFrustum->rightFace) &&
            isOnOrForwardPlane(modelMatrix, camFrustum->topFace) &&
            isOnOrForwardPlane(modelMatrix, camFrustum->bottomFace) &&
            isOnOrForwardPlane(modelMatrix, camFrustum->nearFace) &&
            isOnOrForwardPlane(modelMatrix, camFrustum->farFace));

    //Get global scale thanks to our transform
    //const glm::vec3 globalCenter{ modelMatrix * glm::vec4(center, 1.f) };

    //// Scaled orientation
    //glm::vec3 right = modelTransform.GetRight() * extents.x;
    //glm::vec3 up = modelTransform.GetUp() * extents.y;
    //glm::vec3 forward = modelTransform.GetFront() * extents.z;

    ///*glm::vec3*/ right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    ///*glm::vec3*/ up = glm::normalize(glm::cross(right, forward));

    //const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
    //    std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
    //    std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    //const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
    //    std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
    //    std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    //const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
    //    std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
    //    std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    ////We not need to divise scale because it's based on the half extention of the AABB
    //const nimo::Mesh::OOB globalOOB(globalCenter, newIi, newIj, newIk);

    //return (globalOOB.isOnOrForwardPlane(camFrustum.leftFace) &&
    //        globalOOB.isOnOrForwardPlane(camFrustum.rightFace) &&
    //        globalOOB.isOnOrForwardPlane(camFrustum.topFace) &&
    //        globalOOB.isOnOrForwardPlane(camFrustum.bottomFace) &&
    //        globalOOB.isOnOrForwardPlane(camFrustum.nearFace) &&
    //        globalOOB.isOnOrForwardPlane(camFrustum.farFace));
}

bool nimo::OOB::isOnOrForwardPlane(const glm::mat4& modelMatrix, const Plane& plane) const
{
    int inside = 0;
    for (const auto& vertex : vertices)
    {
        if (plane.getSignedDistanceToPlane(modelMatrix * glm::vec4(vertex, 1.0)) >= 0)
            inside++;
    }

    return inside > 0;
    //// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    //const float r = extents.x * std::abs(plane.normal.x) +
    //    extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

    //return -r <= plane.getSignedDistanceToPlane(center);
}

void nimo::OOB::initVertices(const glm::vec3& min, const glm::vec3& max)
{
    vertices.push_back(max);
    vertices.push_back({ max.x - extents.x * 2, max.y, max.z });
    vertices.push_back({ max.x - extents.x * 2, max.y, max.z - extents.z * 2 });
    vertices.push_back({ max.x, max.y, max.z - extents.z * 2 });
    vertices.push_back(min);
    vertices.push_back({ min.x - extents.x * 2, min.y, min.z });
    vertices.push_back({ min.x - extents.x * 2, min.y, min.z - extents.z * 2 });
    vertices.push_back({ min.x, min.y, min.z - extents.z * 2 });
}
