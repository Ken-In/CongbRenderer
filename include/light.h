#pragma once
#include "glm/glm.hpp"

namespace congb
{
    struct BaseLight
    {
        glm::vec3 color = glm::vec3(1.0f);
        glm::mat4 shadowProjectionMat = glm::mat4(0.0);

        bool changed = false;

        float strength = 1.0f;
        float zNear    = 1.0f;
        float zFar     = 2000.0f;

        unsigned int shadowRes = 1024;
        unsigned int depthMapTextureID = 0;
    };

    struct DirectionalLight : public BaseLight
    {
        glm::vec3 direction = glm::vec3(-1.0f);

        glm::mat4 lightView = glm::mat4(0.0f);
        glm::mat4 lightSpaceMatrix = glm::mat4(0.0);

        float distance;
        float orthoBoxSize;
    };

    struct PointLight : public BaseLight
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::mat4 lookAtPerFace[6];
    };

    struct GPULight{
        glm::vec4 position;
        glm::vec4 color;
        unsigned int enabled;
        float intensity;
        float range;
        float padding;
    };
}
