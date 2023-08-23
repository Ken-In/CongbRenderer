#pragma once
#include "camera.h"
#include "cubeMap.h"
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
        Scene(const std::string &sceneName);
        ~Scene();

        void update(unsigned int deltaTime);

        void drawPointLightShadow(const Shader &pointLightShader, unsigned int index, unsigned int cubeMapTarget);
        void drawDirLightShadow(const Shader &dirLightShader, unsigned int targetTextureID);
        void drawFullScene(const Shader &mainSceneShader);
        void drawDepthPass(const Shader &depthPassShader);

        std::vector<Model*>* getVisibleModels();
        Camera *getCurrentCamera();
        PointLight *getPointLight(unsigned int index);
        unsigned int getShadowRes();

        bool loadingError;
        
        Camera *mainCamera;
        
    private:
        std::string sceneID;
        bool slices = false;

        DirectionalLight dirLight;
        PointLight *pointLights = nullptr;

        std::vector<Model*> visibleModels;
        std::vector<Model*> modelsInScene;

        bool loadContent();
        void generateEnvironmentMaps();
        void loadSkyBox(const json &sceneConfigJson);
        void loadLights(const json &sceneConfigJson);
        void loadCamera(const json &sceneConfigJson);
        void loadSceneModels(const json &sceneConfigJson);

        void frustumCulling();
    };
}

