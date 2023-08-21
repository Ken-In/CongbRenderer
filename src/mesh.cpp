#include "mesh.h"

namespace congb
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices,
        const std::vector<unsigned>& textures)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;

        setupMesh();
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));

        glBindVertexArray(0);
    }

    void Mesh::draw(const Shader& shader, bool textured)
    {
        //Diffuse
        glActiveTexture(GL_TEXTURE0);
        shader.setInt("albedoMap", 0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        if(textured)
        {
            //Emissive
            glActiveTexture(GL_TEXTURE1);
            shader.setInt("emissiveMap", 1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);

            //Normals
            if (textures[2] == 0){
                shader.setBool("normalMapped", false);
            }
            else{
                shader.setBool("normalMapped", true);
            }
            glActiveTexture(GL_TEXTURE2);
            shader.setInt("normalsMap", 2);
            glBindTexture(GL_TEXTURE_2D, textures[2]);

            //Ambient Oclussion
            if (textures[3] == 0){
                shader.setBool("aoMapped", false);
            }
            else{
                shader.setBool("aoMapped", true);
            }
            glActiveTexture(GL_TEXTURE3);
            shader.setInt("lightMap", 3);
            glBindTexture(GL_TEXTURE_2D, textures[3]);

            //Metal / Roughness
            glActiveTexture(GL_TEXTURE4);
            shader.setInt("metalRoughMap", 4);
            glBindTexture(GL_TEXTURE_2D, textures[4]);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    }
}
