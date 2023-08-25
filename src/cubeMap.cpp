﻿#include "cubeMap.h"

#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"

namespace congb
{
    const glm::mat4 CubeMap::captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const glm::mat4 CubeMap::capatureViews[6]  =
        {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
        };
    const std::string CubeMap::fileHandleForFaces[6] = {
        "right.jpg", "left.jpg", "top.jpg",
        "bottom.jpg", "front.jpg", "back.jpg"
    };
    const unsigned int CubeMap::numSidesInCube = 6;
    Cube CubeMap::cubeMapCube = Cube();

    // 正常载入途径：从文件路径下的6个贴图得到cubeMap
    void CubeMap::loadCubeMap(const std::string& folderPath)
    {
        unsigned int ID;    
        std::string filePath = folderPath + "/";

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    
        for(unsigned int i = 0; i < numSidesInCube ; ++i ){
            std::string currentFile = filePath + fileHandleForFaces[i];
            unsigned char *data = stbi_load(currentFile.c_str(), &width, &height, &nComponents, 0);

            if(data){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            else{
                printf("Texture failed to load at path: %s \n", currentFile.c_str());
            }
            stbi_image_free(data);
        }

        //Texture parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        textureID = ID;
        path = filePath;
        type = "cubemap";
    }

    // 非正常载入路径：我们需要生成六张贴图，之后再填充贴图数据
    void CubeMap::generateCubeMap(const int width, const int height, CubeMapType cubeMapType)
    {
        unsigned int ID;
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

        switch(cubeMapType){
            case SHADOW_MAP:
                for (unsigned int i = 0; i < numSidesInCube; ++i){
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0, GL_DEPTH_COMPONENT, width, height, 0,
                                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;

            case HDR_MAP:
                for (unsigned int i = 0; i < numSidesInCube; ++i){
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0, GL_RGB32F,
                                 width, height, 0,
                                 GL_RGB, GL_FLOAT, NULL);
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            break;

            case PREFILTER_MAP:
                for (unsigned int i = 0; i < numSidesInCube; ++i){
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0, GL_RGB16F,
                                 width, height, 0,
                                 GL_RGB, GL_FLOAT, NULL);
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                maxMipLevels = 5;
            break;
        }

        textureID = ID;
        path = "";
        type = "cubemap";
    }

    // IBL diffuse
    void CubeMap::convolveCubeMap(const unsigned environmentMap, const Shader& convolveShader)
    {
        convolveShader.use();
        convolveShader.setInt("environmentMap", 0);
        convolveShader.setMat4("projection", captureProjection);

        glViewport(0, 0, width, height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

        for(unsigned int i = 0; i < numSidesInCube; i++)
        {
            convolveShader.setMat4("view", capatureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            drawCube();
        }
    }

    // IBL specular
    void CubeMap::preFilterCubeMap(const unsigned environmentMap, const unsigned captureRBO, const Shader& filterShader)
    {
    }

    void CubeMap::equiRectangularToCubeMap(const unsigned equirectangularMap, const int resolution,
        const Shader& transformShader)
    {
        transformShader.use();
        transformShader.setInt("equirectangularMap", 0);
        transformShader.setMat4("projection", captureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, equirectangularMap);
        glViewport(0, 0, resolution, resolution);

        for(unsigned int i = 0; i < numSidesInCube; i++)
        {
            transformShader.setMat4("view", capatureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            drawCube();
        }
    }

    void CubeMap::drawCube()
    {
        cubeMapCube.draw();
    }
}
