#include "model.h"
#include "model.h"
#include "geometry.h"
#include "fileManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <string>

namespace congb
{
    Model::Model(const std::string meshPath, const TransformParameters initParameters, bool IBL) : IBL(IBL)
    {
        loadModel(meshPath);
        modelMatrix = glm::mat4(1.0);
        modelMatrix = glm::translate(modelMatrix, initParameters.translation);
        modelMatrix = glm::rotate(modelMatrix, initParameters.angle, initParameters.rotateAxis);
        modelMatrix = glm::scale(modelMatrix, initParameters.scale);
    }

    void Model::loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes |aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

        fileExtension = FLOAD::getFileExtension(path);
        directory = path.substr(0, path.find_last_of('/'));
        directory += '/';

        processNode(scene->mRootNode, scene);
    }

    void Model::update(const unsigned deltaT)
    {
        // todo: model update
    }

    void Model::draw(const Shader& shader, const bool textured)
    {
        shader.setBool("IBL", IBL);
        for(int i = 0; i < meshes.size(); i++)
        {
            meshes[i].draw(shader, textured);
        }
    }

    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.biTangent = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;

            if(mesh->HasTextureCoords(0))
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else
            {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        textures = processTextures(material);

        return Mesh(vertices, indices, textures);
    }

    std::vector<unsigned> Model::processTextures(const aiMaterial* material)
    {
        std::vector<unsigned int> textures;

        aiString texturePath;
        aiTextureType type;
        std::string fullTexturePath;

        for(int tex = aiTextureType_NONE; tex <= aiTextureType_UNKNOWN; tex++)
        {
            type = static_cast<aiTextureType>(tex);
            fullTexturePath = directory;

            if(material->GetTextureCount(type) > 0)
            {
                material->GetTexture(type, 0, &texturePath);
                fullTexturePath = fullTexturePath.append(texturePath.C_Str());

                if(textureAtlas.count(fullTexturePath) == 0)
                {
                    Texture texture;
                    bool srgb = tex == aiTextureType_DIFFUSE;
                    texture.loadTexture(fullTexturePath, srgb);
                    textureAtlas.insert({fullTexturePath, texture});
                }

                textures.push_back(textureAtlas.at(fullTexturePath).textureID);
            }
            else
            {
                switch (type)
                {
                case aiTextureType_LIGHTMAP:
                case aiTextureType_EMISSIVE:
                case aiTextureType_NORMALS:
                case aiTextureType_UNKNOWN:
                    textures.push_back(0);
                    break;
                }
            }
        }
        return textures;
    }
}
