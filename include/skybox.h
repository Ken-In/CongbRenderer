#pragma once
#include <string>

#include "cubeMap.h"
#include "shader.h"
#include "texture.h"

namespace congb
{
    struct Skybox
    {
    public:
        void draw();

        void setup(const std::string &skyboxName, bool isHDR, int resolution);

        void fillCubeMapWithTexture(const Shader &buildCubeMapShader);

        unsigned int resolution;

        Texture equirectangleMap;
        CubeMap skyBoxCubeMap;
    };
}


