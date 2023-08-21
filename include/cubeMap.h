#pragma once
#include "texture.h"

namespace congb
{
    enum CubeMapType
    {
        SHADOW_MAP,
        HDR_MAP,
        PREFILTER_MAP
    };
    
    struct CubeMap : public Texture
    {
    public:
        void loadCubeMap(const std::string &folderPath);
        void generateCubeMap(const int width, const int height, CubeMapType cubeMapType);

        void convolveCubeMap(const unsigned int environmentMap, const Shader &convolveShader);
        void preFilterCubeMap(const unsigned int environmentMap, const unsigned int captureRBO,const Shader &filterShader);
        void equiRectangularToCubeMap(const unsigned int equirectangularMap, const int resolution, const Shader &transformShader);

        void drawCube();

        unsigned int maxMipLevels;

        static Cube 
    };
}
