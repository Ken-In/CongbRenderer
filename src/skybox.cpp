#include "skybox.h"

namespace congb
{
    void Skybox::draw()
    {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxCubeMap.textureID);
        skyBoxCubeMap.drawCube();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void Skybox::setup(const std::string& skyboxName, bool HDR, int res)
    {
        std::string skyboxFolderPath = "../assets/skyboxes/";
        skyboxFolderPath += skyboxName;
        std::string skyboxFilePath = skyboxFolderPath + "/" + skyboxName + ".hdr";

        resolution = res;
        isHDR = HDR;
        if(isHDR)
        {
            equirectangleMap.loadHDRTexture(skyboxFilePath);
            skyBoxCubeMap.generateCubeMap(res, res, HDR_MAP);
        }
        else
        {
            skyBoxCubeMap.loadCubeMap(skyboxFolderPath);
        }
    }

    void Skybox::fillCubeMapWithTexture(const Shader& buildCubeMapShader)
    {
        skyBoxCubeMap.equiRectangularToCubeMap(equirectangleMap.textureID, resolution, buildCubeMapShader);
    }
}
