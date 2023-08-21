#pragma once
#include "camera.h"
#include "light.h"
#include "model.h"
#include "shader.h"
#include "nlohmann/json.hpp"


using json = nlohmann::json;

namespace congb
{
    class Scene
    {
    public:
        Scene(const std::string &sceneFolder);
        ~Scene();

        void update(unsigned int deltaTime);

        void drawPointLightShadow(const Shader &pointLightShader, unsigned int index, unsigned int cubeMapTarget);
        void drawDirLightShadow(const Shader &dirLightShader, unsigned int targetTextureID);
        void drawFullScene(const Shader &mainSceneShader, const Shader &skyboxShader);
        void drawDepthPass(const Shader &depthPassShader);

        std::vector<Model*> getVisibleModels();
        Camera *getCurrentCamera();
        PointLight *getPointLight(unsigned int index);
        unsigned int getShadowRes();

        
    };
}

