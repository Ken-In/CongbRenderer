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
        //Current light
        PointLight * light = &pointLights[index];
        light->depthMapTextureID = cubeMapTarget;
        //Shader setup
        pointLightShader.use();
        pointLightShader.setVec3("lightPos", light->position);
        pointLightShader.setFloat("far_plane", light->zFar);

        //Matrix setup
        glm::mat4 lightMatrix, M;
        glm::mat4 shadowProj = light->shadowProjectionMat;
        for (unsigned int face = 0; face < 6; ++face){
            std::string number = std::to_string(face);
            lightMatrix = shadowProj * light->lookAtPerFace[face];
            pointLightShader.setMat4(("shadowMatrices[" + number + "]").c_str(), lightMatrix);
        }

        for(unsigned int i = 0; i < modelsInScene.size(); ++i){
            Model * currentModel = modelsInScene[i];

            M = currentModel->modelMatrix;
            //Shader setup stuff that changes every frame
            pointLightShader.setMat4("M", M);
        
            //Draw object
            currentModel->draw(pointLightShader, false);
        }
    }

    void Scene::drawDirLightShadow(const Shader& dirLightShader, unsigned targetTextureID)
    {
        glm::mat4 ModelLS = glm::mat4(1.0);
        dirLight.depthMapTextureID = targetTextureID;

        float left   = -dirLight.orthoBoxSize;
        float right  = dirLight.orthoBoxSize;
        float bottom = -dirLight.orthoBoxSize;
        float top    = dirLight.orthoBoxSize;
        dirLight.shadowProjectionMat = glm::ortho(left, right, bottom, top, dirLight.zNear, dirLight.zFar);
        dirLight.lightView = glm::lookAt(40.0f * -dirLight.direction,
                                         glm::vec3(0.0f, 0.0f, 0.0f),
                                         glm::vec3(0.0f, 1.0f, 0.0f));

        dirLight.lightSpaceMatrix = dirLight.shadowProjectionMat * dirLight.lightView;

        //Drawing every object into the shadow buffer
        for(unsigned int i = 0; i < modelsInScene.size(); ++i){
            Model * currentModel = modelsInScene[i];

            //Matrix setup
            ModelLS = dirLight.lightSpaceMatrix * currentModel->modelMatrix;

            //Shader setup stuff that changes every frame
            dirLightShader.use();
            dirLightShader.setMat4("lightSpaceMatrix", ModelLS);
        
            //Draw object
            currentModel->draw(dirLightShader, false);
        }
    }

    void Scene::drawFullScene(const Shader& mainSceneShader, const Shader& skyboxShader)
    {
        glm::mat4 MVP   = glm::mat4(1.0);
        glm::mat4 M     = glm::mat4(1.0);
        glm::mat4 VP    = mainCamera->projectionMatrix * mainCamera->viewMatrix;
        glm::mat4 VPCubeMap = mainCamera->projectionMatrix *glm::mat4(glm::mat3(mainCamera->viewMatrix));

        const unsigned int numTextures = 5;

        // todo : ui

        // set lights
        mainSceneShader.use();
        mainSceneShader.setVec3("dirLight.direction", dirLight.direction);
        mainSceneShader.setBool("slices", slices);
        mainSceneShader.setVec3("dirLight.color",   dirLight.strength * dirLight.color);
        mainSceneShader.setMat4("lightSpaceMatrix", dirLight.lightSpaceMatrix);
        mainSceneShader.setVec3("cameraPos_wS", mainCamera->position);
        mainSceneShader.setInt("light_count", pointLightCount);
        for (unsigned int i = 0; i < pointLightCount; ++i)
        {
            PointLight *light = &pointLights[i];
            std::string number = std::to_string(i);

            glActiveTexture(GL_TEXTURE0 + numTextures + i); 
            mainSceneShader.setInt(("depthMaps[" + number + "]").c_str(), numTextures + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthMapTextureID);
            mainSceneShader.setFloat("far_plane", light->zFar);
        }
        
        //Direction light shadow map
        glActiveTexture(GL_TEXTURE0 + numTextures + pointLightCount);
        mainSceneShader.setInt("shadowMap", numTextures + pointLightCount);
        glBindTexture(GL_TEXTURE_2D, dirLight.depthMapTextureID);

        for(unsigned int i = 0; i < visibleModels.size(); ++i)
        {
            Model *currentModel = visibleModels[i];

            M   = currentModel->modelMatrix;
            MVP = VP * M;

            mainSceneShader.setMat4("MVP", MVP);
            mainSceneShader.setMat4("M", M);

            currentModel->draw(mainSceneShader, true);
        }

        // todo : skybox
        skyboxShader.use();
        skyboxShader.setMat4("VP", VPCubeMap);
        mainSkybox.draw();
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
        printf("Loading models...\n");
        CubeMap::cubeMapCube.setup();
        loadSkyBox(configJson);
        
        // todo: lights
        printf("Loading lights...\n");
        loadLights(configJson);
        
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
        json skyBox = sceneConfigJson["skybox"];
        std::string skyBoxName = skyBox["id"];
        bool isHDR = skyBox["hdr"];
        int resolution = skyBox["resolution"];

        mainSkybox.setup(skyBoxName, isHDR, resolution);
    }

    void Scene::loadLights(const json& sceneConfigJson)
    {
        //Directional light
        printf("Loading directional light...\n");
        {
            json light = sceneConfigJson["directionalLight"];

            json direction = light["direction"];
            dirLight.direction = glm::vec3((float)direction[0],
                                            (float)direction[1],
                                            (float)direction[2]);

            json color = light["color"];
            dirLight.color = glm::vec3((float)color[0],
                                            (float)color[1],
                                            (float)color[2]);
                                        
            //Scalar values
            dirLight.distance = (float)light["distance"];
            dirLight.strength = (float)light["strength"];
            dirLight.zNear = (float)light["zNear"];
            dirLight.zFar = (float)light["zFar"];
            dirLight.orthoBoxSize = (float)light["orthoSize"];
            dirLight.shadowRes = (unsigned int)light["shadowRes"];

            float left   = -dirLight.orthoBoxSize;
            float right  = dirLight.orthoBoxSize;
            float bottom = -dirLight.orthoBoxSize;
            float top    = dirLight.orthoBoxSize;
            dirLight.shadowProjectionMat = glm::ortho(left, right, bottom, top, dirLight.zNear, dirLight.zFar);
            dirLight.lightView = glm::lookAt(dirLight.distance * -dirLight.direction,
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, 1.0f, 0.0f));

            dirLight.lightSpaceMatrix = dirLight.shadowProjectionMat * dirLight.lightView;
        }
        //Point lights
        printf("Loading point light...\n");
        {
            //Get number of lights in scene and initialize array containing them
            pointLightCount = (unsigned int)sceneConfigJson["pointLights"].size();
            pointLights = new PointLight[pointLightCount];

            for(unsigned int i = 0; i < pointLightCount; ++i){
                json light = sceneConfigJson["pointLights"][i];

                json position = light["position"];
                pointLights[i].position = glm::vec3((float)position[0],
                                                    (float)position[1],
                                                    (float)position[2]);

                json color = light["color"];
                pointLights[i].color = glm::vec3((float)color[0],
                                                 (float)color[1],
                                                 (float)color[2]);

                //Scalar values
                pointLights[i].strength = (float)light["strength"];
                pointLights[i].zNear = (float)light["zNear"];
                pointLights[i].zFar = (float)light["zFar"];
                pointLights[i].shadowRes = (unsigned int)light["shadowRes"];

                //Matrix setup
                pointLights[i].shadowProjectionMat = glm::perspective(glm::radians(90.0f), 1.0f,
                                                                      pointLights[i].zNear,
                                                                      pointLights[i].zFar);
            
                glm::vec3 lightPos = pointLights[i].position;
                pointLights[i].lookAtPerFace[0] = glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
                pointLights[i].lookAtPerFace[1] = glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
                pointLights[i].lookAtPerFace[2] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
                pointLights[i].lookAtPerFace[3] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
                pointLights[i].lookAtPerFace[4] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
                pointLights[i].lookAtPerFace[5] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
            }
        }
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
