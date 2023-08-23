#include "scene.h"

#include <fstream>

#include "fileManager.h"

namespace congb
{
    Scene::Scene(const std::string& sceneName)
    {
        std::string folderPath = "../assets/scenes/";
        std::string fileExtension = ".json";
        sceneID = sceneName;

        printf("\nBeginning Scene load, checking scene description file:\n");
        if( !FLOAD::checkFileValidity(folderPath + sceneName + fileExtension) ){
            //If you do not find the scene file set the quit flag to true 
            printf("Cannot find scene descriptor file for %s \n", sceneID.c_str());
            loadingError = true; 
        }
        else{
            //Load all cameras, models and lights and return false if it fails
            loadingError = !loadContent();
        }
    }

    Scene::~Scene()
    {
        if(!loadingError)
        {
            for(Model* model : modelsInScene)
            {
                delete model;
            }
            if(pointLights)
            {
                delete []pointLights;
            }
            delete mainCamera;
        }
    }

    void Scene::update(unsigned deltaTime)
    {
        visibleModels.clear();
        mainCamera->update(deltaTime);
        for(Model* model : modelsInScene)
        {
            model->update(deltaTime);
        }
        frustumCulling();
    }

    void Scene::drawPointLightShadow(const Shader& pointLightShader, unsigned index, unsigned cubeMapTarget)
    {
    }

    void Scene::drawDirLightShadow(const Shader& dirLightShader, unsigned targetTextureID)
    {
    }

    void Scene::drawFullScene(const Shader& mainSceneShader)
    {
        glm::mat4 MVP   = glm::mat4(1.0);
        glm::mat4 M     = glm::mat4(1.0);
        glm::mat4 VP    = mainCamera->projectionMatrix * mainCamera->viewMatrix;

        const unsigned int numTextures = 5;

        // todo : ui

        // todo : add dirLight and pointLights

        
        mainSceneShader.use();

        for(unsigned int i = 0; i < visibleModels.size(); ++i)
        {
            Model *currentModel = visibleModels[i];

            M   = currentModel->modelMatrix;
            MVP = VP * M;

            mainSceneShader.setMat4("MVP", MVP);
            mainSceneShader.setMat4("M", M);

            currentModel->draw(mainSceneShader, false);
        }

        // todo : skybox
        
    }

    void Scene::drawDepthPass(const Shader& depthPassShader)
    {
    }

    std::vector<Model*>* Scene::getVisibleModels()
    {
        return &visibleModels;
    }

    Camera* Scene::getCurrentCamera()
    {
        return mainCamera;
    }

    PointLight* Scene::getPointLight(unsigned index)
    {
        return &pointLights[index];
    }

    unsigned Scene::getShadowRes()
    {
        return dirLight.shadowRes;
    }

    bool Scene::loadContent()
    {
        std::string folderPath = "../assets/scenes/";
        std::string fileExtension = ".json";
        std::string sceneConfigFilePath = folderPath + sceneID + fileExtension;
        std::ifstream file(sceneConfigFilePath.c_str());
        json configJson;
        file >> configJson;

        if(configJson["sceneID"] != sceneID)
        {
            printf("Error! Config file: %s does not belong to current scene, check configuration.\n", sceneConfigFilePath.c_str());
            return false;
        }
        if((unsigned int)configJson["models"].size() == 0)
        {
            printf("Error! No models found in the configuration file : %s.\n", sceneConfigFilePath.c_str());
            return false;
        }

        printf("Loading camera...\n");
        loadCamera(configJson);

        printf("Loading models...\n");
        loadSceneModels(configJson);

        // todo: skybox

        // todo: lights

        // todo: enviroment map

        printf("Loading Complete!...\n");
        bool res = modelsInScene.empty();
        return !res;
    }

    void Scene::generateEnvironmentMaps()
    {
    }

    void Scene::loadSkyBox(const json& sceneConfigJson)
    {
    }

    void Scene::loadLights(const json& sceneConfigJson)
    {
    }

    void Scene::loadCamera(const json& sceneConfigJson)
    {
        json cameraSettings = sceneConfigJson["camera"];
        float speed = (float)cameraSettings["speed"];
        float sens  = (float)cameraSettings["mouseSens"];
        float fov   = (float)cameraSettings["fov"];
        float nearP = (float)cameraSettings["nearPlane"];
        float farP  = (float)cameraSettings["farPlane"];

        json position = cameraSettings["position"];
        glm::vec3 pos = glm::vec3((float)position[0], (float)position[1], (float)position[2]);

        json target   = cameraSettings["target"];
        glm::vec3 tar = glm::vec3((float)target[0], (float)target[1], (float)target[2]);

        mainCamera = new Camera(tar, pos, fov, speed, sens, nearP, farP);
    }

    void Scene::loadSceneModels(const json& sceneConfigJson)
    {
        std::string modelMesh, modelName;
        TransformParameters initParameters;
        bool IBL;
        unsigned int modelCount = (unsigned int)sceneConfigJson["models"].size();

        for(unsigned int i = 0; i < modelCount; i++)
        {
            json currentModel = sceneConfigJson["models"][i];
            modelMesh = currentModel["mesh"].get<std::string>();
            IBL = currentModel["IBL"];

            modelName = modelMesh.substr(0, modelMesh.find_last_of('.'));

            json position = currentModel["position"];
            initParameters.translation = glm::vec3((float)position[0], (float)position[1], (float)position[2]);

            json rotation = currentModel["rotation"];
            initParameters.angle = glm::radians((float)rotation[0]);
            initParameters.rotateAxis = glm::vec3((float)rotation[1],
                                                (float)rotation[2],
                                                (float)rotation[3]);

            json scaling = currentModel["scaling"];
            initParameters.scale = glm::vec3((float)scaling[0], (float)scaling[1], (float)scaling[2]);

            modelMesh = "../assets/models/" + modelName + "/" + modelMesh;
            if (!FLOAD::checkFileValidity(modelMesh)){
                printf("Error! Mesh: %s does not exist.\n", modelMesh.c_str());
            }
            else{
                modelsInScene.push_back(new Model(modelMesh, initParameters, IBL));
            }
        }
    }

    void Scene::frustumCulling()
    {
        for(Model* model : modelsInScene)
        {
            visibleModels.push_back(model);
            // todo: frustumCulling
        }
    }
}
