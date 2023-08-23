#pragma once

#include "mesh.h"
#include "shader.h"
#include "glm/glm.hpp"
#include "assimp/scene.h"
#include <unordered_map>

namespace congb
{
    struct TransformParameters
    {
        TransformParameters() : scale(glm::vec3(1.0f)){}
        glm::vec3 translation;
        float angle;
        glm::vec3 rotateAxis;
        glm::vec3 scale;
    };
    
    struct Model
    {
    public:
        Model(const std::string meshPath, const TransformParameters initParameters, bool IBL);

        void loadModel(std::string path);
        void update(const unsigned int deltaT);
        void draw(const Shader &shader, const bool textured);

        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<unsigned int> processTextures(const aiMaterial *material);

        bool IBL;
        glm::mat4 modelMatrix;
        std::vector<Mesh> meshes;

        std::unordered_map<std::string, Texture> textureAtlas;
        std::string directory, fileExtension;
    };
}
