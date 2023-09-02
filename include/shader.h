#pragma once

#include <glad/glad.h>
#include <string>
#include "glm/glm.hpp"

namespace congb
{
    struct Shader{
        bool setup(const std::string vertexPath, const std::string fragmentPath,
               const std::string geometryPath = "");

        ~Shader(){
            glDeleteProgram(ID);
        }

        void use() const;

        void setBool(const std::string &name, bool value) const; 
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        void setVec3(const std::string &name, const glm::vec3 &vec) const;

        unsigned int ID;
    };

    struct ComputeShader : public Shader
    {
        bool setup(const std::string computePath);
        void dispatch(unsigned int x, unsigned int y = 1, unsigned int z = 1) const;
    };
}
