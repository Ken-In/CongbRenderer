#pragma once

#include <string>
#include "shader.h"

namespace congb
{
    enum TextureType{
        MULT_2D_HDR_COL,
        SING_2D_HDR_COL,
        MULT_2D_HDR_DEP,
        SING_2D_HDR_DEP,
        SING_2D_HDR_COL_CLAMP,
        SING_2D_HDR_DEP_BORDER
    };

    struct Texture{
        void loadTexture(const std::string &filePath, bool sRGB);
        void loadHDRTexture(const std::string &filePath);
        unsigned int loadDDSTexture(char const* Filename);

        //Allocate space for a texure directly on the gpu and return an ID
        static unsigned int genTextureDirectlyOnGPU(const int width, const int height,
                                                    const unsigned int attachmentNum, TextureType type);

        //TextureID is zero only if the texture has not been initialized properly
        //by OpenGl
        unsigned int textureID = 0;
        int width, height, nComponents;

        //Leftover from old non-pbr workflow. TODO:: cleanup and/or recuperate functionality
        std::string type, path;
    };
}
