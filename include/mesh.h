#pragma once

#include "glm/glm.hpp"
#include "shader.h"
#include "texture.h"
#include <vector>
#include <string>

namespace congb
{
    struct Vertex{
        glm::vec3 position; 
        glm::vec3 normal; 
        glm::vec3 tangent; 
        glm::vec3 biTangent; 
        glm::vec2 texCoords; 
    };

    struct Mesh {
        //Supposedly STL is not recommended for performance, but I don't believe this is the case
        //in our current implementation
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> textures;

        Mesh(const std::vector<Vertex> &vertices,
             const std::vector<unsigned int> &indices,
             const std::vector<unsigned int> &textures)
        {
            this->vertices = vertices;
            this->indices  = indices;
            this->textures = textures;

            setupMesh();
        }

        void setupMesh();
        void draw(const Shader &shader, bool textured);

        //OpenGL drawing variables
        unsigned int VAO, VBO, EBO;
    };
}
