#include "texture.h"
#include "gli/gli.hpp"
#include "fileManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>

namespace congb
{
    void  Texture::loadTexture(const std::string& filePath, bool sRGB)
    {
        path = filePath;
        std::replace(path.begin(), path.end(), '\\', '/');
        std::string fileExtension = FLOAD::getFileExtension(filePath);

        if(fileExtension == "dds")
        {
            textureID = loadDDSTexture(path.c_str());
        }
        else
        {
            unsigned int ID;
            glGenTextures(1, &ID);
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nComponents, 0);
            if (data)
            {
                GLenum format;
                GLenum internalFormat;
                if(nComponents == 1)
                {
                    format = GL_RED;
                    internalFormat = GL_RED;
                }
                else if(nComponents == 3)
                {
                    format = GL_RGB;
                    if(sRGB)
                    {
                        internalFormat = GL_SRGB;
                    }
                    else
                    {
                        internalFormat = GL_RGB;
                    }
                }
                else if(nComponents == 4)
                {
                    format = GL_RGBA;
                    if(sRGB)
                    {
                        internalFormat = GL_SRGB_ALPHA;
                    }
                    else
                    {
                        internalFormat = GL_RGBA;
                    }
                }

                glBindTexture(GL_TEXTURE_2D, ID);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                //MipMapped and repeating
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                printf("Texture failed to load at path: %s \n", path.c_str());
            }
            textureID = ID;
        }
    }
    
    void Texture::loadHDRTexture(const std::string& filePath)
    {
        stbi_set_flip_vertically_on_load(true);

        float* data = stbi_loadf(filePath.c_str(), &width, &height, &nComponents, 0);
        if(data)
        {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            printf("Texture failed to load at path: %s \n", filePath.c_str());
        }
        stbi_image_free(data);
    }

    unsigned Texture::loadDDSTexture(char const* Filename)
    {
        gli::texture texture = gli::load(Filename);
        if(texture.empty())
        {
            return 0;
        }

        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
        GLenum target = GL.translate(texture.target());

        unsigned int textureName = 0;
        glGenTextures(1, &textureName);
        glBindTexture(target, textureName);
        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

        glm::tvec3<GLsizei> const extent(texture.extent());
        GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

        switch (texture.target())
        {
        case gli::TARGET_1D:
            glTexStorage1D(
                target, static_cast<GLint>(texture.levels()), format.Internal, extent.x);
            break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            glTexStorage2D(
                target, static_cast<GLint>(texture.levels()), format.Internal,
                extent.x, texture.target() == gli::TARGET_2D ? extent.y : faceTotal);
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            glTexStorage3D(
                target, static_cast<GLint>(texture.levels()), format.Internal,
                extent.x, extent.y,
                texture.target() == gli::TARGET_3D ? extent.z : faceTotal);
            break;
        default:
            assert(0);
            break;
        }

        for(std::size_t layer = 0; layer < texture.layers(); ++layer)
        for(std::size_t face = 0; face < texture.faces(); ++face)
        for(std::size_t level = 0; level < texture.levels(); ++level)
        {
            GLsizei const layerGL = static_cast<GLsizei>(layer);
            glm::tvec3<GLsizei> extent(texture.extent(level));
            target = gli::is_target_cube(texture.target())
                ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
                : target;

            switch (texture.target())
            {
            case gli::TARGET_1D:
                if(gli::is_compressed(texture.format()))
                    glCompressedTexSubImage1D(
                        target, static_cast<GLint>(level), 0, extent.x,
                        format.Internal, static_cast<GLsizei>(texture.size(level)),
                        texture.data(layer, face, level));
                else
                    glTexSubImage1D(
                        target, static_cast<GLint>(level), 0, extent.x,
                        format.External, format.Type,
                        texture.data(layer, face, level));
                break;
            case gli::TARGET_1D_ARRAY:
            case gli::TARGET_2D:
            case gli::TARGET_CUBE:
                if(gli::is_compressed(texture.format()))
                    glCompressedTexSubImage2D(
                        target, static_cast<GLint>(level),
                        0, 0,
                        extent.x,
                        texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
                        format.Internal, static_cast<GLsizei>(texture.size(level)),
                        texture.data(layer, face, level));
                else
                    glTexSubImage2D(
                        target, static_cast<GLint>(level),
                        0, 0,
                        extent.x,
                        texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
                        format.External, format.Type,
                        texture.data(layer, face, level));
                break;
            case gli::TARGET_2D_ARRAY:
            case gli::TARGET_3D:
            case gli::TARGET_CUBE_ARRAY:
                if(gli::is_compressed(texture.format()))
                    glCompressedTexSubImage3D(
                        target, static_cast<GLint>(level),
                        0, 0, 0,
                        extent.x, extent.y,
                        texture.target() == gli::TARGET_3D ? extent.z : layerGL,
                        format.Internal, static_cast<GLsizei>(texture.size(level)),
                        texture.data(layer, face, level));
                else
                    glTexSubImage3D(
                        target, static_cast<GLint>(level),
                        0, 0, 0,
                        extent.x, extent.y,
                        texture.target() == gli::TARGET_3D ? extent.z : layerGL,
                        format.External, format.Type,
                        texture.data(layer, face, level));
                break;
            default: assert(0); break;
            }
        }
        return textureName;
    }

    unsigned Texture::genTextureDirectlyOnGPU(const int width, const int height, const unsigned attachmentNum,
        TextureType type)
    {
        unsigned int genTextureID; 
        float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glGenTextures(1, &genTextureID);
        switch(type)
        {
        case MULT_2D_HDR_COL:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, genTextureID);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNum, GL_TEXTURE_2D_MULTISAMPLE, genTextureID, 0);
            return genTextureID;
        case SING_2D_HDR_COL:
            glBindTexture(GL_TEXTURE_2D, genTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNum, GL_TEXTURE_2D, genTextureID, 0);
            return genTextureID;
        case MULT_2D_HDR_DEP:
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, genTextureID);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32F, width, height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, genTextureID, 0);
            return genTextureID;
        case SING_2D_HDR_DEP:
            glBindTexture(GL_TEXTURE_2D, genTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, genTextureID, 0);
            return genTextureID;
        case SING_2D_HDR_COL_CLAMP:
            glBindTexture(GL_TEXTURE_2D, genTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNum, GL_TEXTURE_2D, genTextureID, 0);
            return genTextureID;
        case SING_2D_HDR_DEP_BORDER:
            glBindTexture(GL_TEXTURE_2D, genTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, genTextureID, 0);
            return genTextureID;
        default:
            return 0;
        }
    }
}
