#include "renderManager.h"

#include <random>

#include "gpuData.h"
namespace congb
{
    RenderManager::RenderManager()
    {
    }

    RenderManager::~RenderManager()
    {
    }

    bool RenderManager::startUp(DisplayManager& displayManager, SceneManager& sceneManager)
    {
        printf("\nInitializing Renderer.\n");
        screen          = &displayManager;
        sceneLocator    = &sceneManager;
        currentScene    = sceneLocator->getCurrentScene();
        sceneCamera     = currentScene->getCurrentCamera();

        printf("Loading FBO's...\n");
        if(!initFBOs())
        {
            printf("FBO's failed to be initialized correctly.\n");
            return false;
        }
        
        printf("Loading Shaders...\n");
        if(!loadShaders())
        {
            printf("Shaders failed to be initialized correctly.\n");
            return false;
        }

        printf("Loading SSBO's...\n");
        if (!initSSBOs()){
            printf("SSBO's failed to be initialized correctly.\n");
            return false;
        }

        printf("Preprocessing...\n");
        if (!preProcess()){
            printf("SSBO's failed to be initialized correctly.\n");
            return false;
        }

        printf("Renderer Initialization complete.\n");
        return true;
    }

    void RenderManager::shutDown()
    {
        /*glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);*/
        
        screen      = nullptr;
        sceneCamera = nullptr;
        sceneLocator= nullptr;
    }

    void RenderManager::render()
    {
        //Direction light shadow map
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);

        dirShadowFBO.bind();
        dirShadowFBO.clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
        currentScene->drawDirLightShadow(dirShadowShader, dirShadowFBO.depthBuffer);
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        multiSampledFBO.bind();

        // 计算 cluster 的光照
        cullLightsCompShader.use();
        cullLightsCompShader.setMat4("viewMatrix", sceneCamera->viewMatrix);
        cullLightsCompShader.dispatch(1,1,6); 
        
        multiSampledFBO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec3(0.0f));
        currentScene->drawFullScene(simpleShader, skyboxShader);

        multiSampledFBO.blitTo(simpleFBO, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        postProcess();
        
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("OpenGL error：%d\n", error);
        }
        screen->swapDisplayBuffer();
    }

    bool RenderManager::initFBOs()
    {
        bool stillValid = true;

        //Direction light shadow map
        unsigned int shadowMapResolution = currentScene->getShadowRes();
        stillValid &= dirShadowFBO.setupFrameBuffer(shadowMapResolution, shadowMapResolution);
        
        //Point light shadow map
        numLights = currentScene->pointLightCount;
        // 为了防止shadow map爆炸 最多4个shadowMap
        unsigned int pointLightShadowNum = numLights > 4 ? 4 : numLights;
        pointLightShadowFBOs = new PointShadowBuffer[pointLightShadowNum];
        for(unsigned int i = 0; i < pointLightShadowNum; ++i ){
            stillValid &= pointLightShadowFBOs[i].setupFrameBuffer(shadowMapResolution, shadowMapResolution);
        }

        if(!stillValid){
            printf("Error initializing shadow map FBO's!\n");
            return false;
        }
        
        //Rendering buffers
        int skyboxRes = currentScene->mainSkybox.resolution;
        stillValid &= captureFBO.setupFrameBuffer(skyboxRes, skyboxRes);
        stillValid &= multiSampledFBO.setupFrameBuffer();

        if(!stillValid){
            printf("Error initializing rendering FBO's!\n");
            return false;
        }

        //Post processing buffers
        stillValid &= pingPongFBO.setupFrameBuffer();
        stillValid &= simpleFBO.setupFrameBuffer();

        if(!stillValid){
            printf("Error initializing postPRocessing FBO's!\n");
            return false;
        }
        
        return stillValid;
    }

    bool RenderManager::initSSBOs()
    {
        // 设置包含所有 cluster 的 AABB buffer
        {
            glGenBuffers(1, &AABBvolumeGridSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, AABBvolumeGridSSBO);

            //We generate the buffer but don't populate it yet.
            glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AABBvolumeGridSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        sizeX =  (unsigned int)std::ceilf(DisplayManager::SCREEN_WIDTH / (float)gridSizeX);
        sizeY =  (unsigned int)std::ceilf(DisplayManager::SCREEN_HEIGHT / (float)gridSizeY);
        
        float zFar    =  sceneCamera->cameraFrustum.farPlane;
        float zNear   =  sceneCamera->cameraFrustum.nearPlane;

        // 设置 screen2View ssbo, 传入转换空间需要的数据
        {
            glGenBuffers(1, &screenToViewSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, screenToViewSSBO);

            //Setting up contents of buffer
            screen2View.inverseProjectionMat = glm::inverse(sceneCamera->projectionMatrix);
            screen2View.tileSizeX = gridSizeX;
            screen2View.tileSizeY = gridSizeY;
            screen2View.tileSizeZ = gridSizeZ;
            screen2View.tilePixelSize.x = 1.0f / (float)sizeX;
            screen2View.tilePixelSize.y = 1.0f / (float)sizeY;
            screen2View.viewPixelSize.x = 1.0f / (float)DisplayManager::SCREEN_WIDTH;
            screen2View.viewPixelSize.y = 1.0f / (float)DisplayManager::SCREEN_HEIGHT;
            //Basically reduced a log function into a simple multiplication an addition by pre-calculating these
            screen2View.sliceScalingFactor = static_cast<float>(gridSizeZ) / std::log2f(zFar / zNear) ;
            screen2View.sliceBiasFactor    = -(static_cast<float>(gridSizeZ) * std::log2f(zNear) / std::log2f(zFar / zNear)) ;

            //Generating and copying data to memory in GPU
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView), &screen2View, GL_STATIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screenToViewSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        
        // 设置包含场景中所有点光源的light buffer
        {
            glGenBuffers(1, &lightSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

            GLint bufMask = GL_READ_WRITE;

            struct GPULight *lights = (struct GPULight *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);
            PointLight *light;
            std::random_device rd; // 用于获取种子
            std::mt19937 gen(rd()); // 使用种子初始化梅森旋转发生器
            std::uniform_real_distribution<> dis(5.0, 20.0); 
            for(unsigned int i = 0; i < numLights; ++i ){
                //Fetching the light from the current scene
                light = currentScene->getPointLight(i);
                lights[i].position  = glm::vec4(light->position, 1.0f);
                lights[i].color     = glm::vec4(light->color, 1.0f);
                lights[i].enabled   = 1; 
                lights[i].intensity = 1.0f;
                lights[i].range     = static_cast<float>(dis(gen));
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        // lightIndexList 记录 cluster 的光源索引，但排序是随机的
        {
            unsigned int totalNumLights =  numClusters * maxLightsPerTile; //3456 * maxLightsPerTile * 4
            glGenBuffers(1, &lightIndexListSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexListSSBO);

            glBufferData(GL_SHADER_STORAGE_BUFFER,  totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexListSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        
        // light grid 按顺序存储每个 cluster 的光源信息，和lightIndexList对应
        {
            glGenBuffers(1, &lightGridSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGridSSBO);

            // 每个光源记录一个光源数 count 和在 lightIndexList 的偏移 offset
            glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        // 设置一个 int 记录所有 cluster 的光源数总和
        {
            glGenBuffers(1, &lightIndexGlobalCountSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexGlobalCountSSBO);

            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCountSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        return true;
    }

    bool RenderManager::loadShaders()
    {
        bool success = true;
        // test shader
        success &= helloTriangleShader.setup("1_helloTriangle.vert", "1_helloTriangle.frag", "");
        success &= simpleShader.setup("2_simpleShader.vert", "2_simpleShader.frag", "");

        // preProcess
        success &= buildAABBGridCompShader.setup("6_clusterShader.comp");
        success &= cullLightsCompShader.setup("6_clusterCullLightShader.comp");
        
        if(!success){
            printf("Error loading pre-processing Shaders!\n");
            return false;
        }
        
        // skybox shader
        success &= skyboxShader.setup("3_skyboxShader.vert", "3_skyboxShader.frag", "");
        success &= fillCubeMapShader.setup("3_buildCubeMapShader.vert", "3_buildCubeMapShader.frag");
        
        
        // shadow map shader
        success &= dirShadowShader.setup("5_shadowShader.vert", "5_shadowShader.frag");
        success &= pointShadowShader.setup("5_pointShadowShader.vert", "5_pointShadowShader.frag", "5_pointShadowShader.geom");
        
        // post process shader
        success &= screenSpaceShader.setup("4_screenShader.vert", "4_screenShader.frag");
        success &= highPassFilterShader.setup("4_splitHighShader.vert", "4_splitHighShader.frag");
        success &= gaussianBlurShader.setup("4_blurShader.vert", "4_blurShader.frag");
        
        if(!success)
        {
            printf("Error loading Shaders!\n");
            return false;
        }
        
        return true;
    }

    bool RenderManager::preProcess()
    {
        glDisable(GL_BLEND);

        canvas.setup();

        // build AABB computer shader
        buildAABBGridCompShader.use();
        buildAABBGridCompShader.setFloat("zNear", sceneCamera->cameraFrustum.nearPlane);
        buildAABBGridCompShader.setFloat("zFar", sceneCamera->cameraFrustum.farPlane);
        buildAABBGridCompShader.dispatch(gridSizeX, gridSizeY, gridSizeZ);
        
        // fill skybox
        captureFBO.bind();
        if(currentScene->mainSkybox.isHDR)
        {
            currentScene->mainSkybox.fillCubeMapWithTexture(fillCubeMapShader);
        }

        // draw point light shadow maps
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        unsigned int shadowNum = currentScene->pointLightCount > 4 ? 4 : currentScene->pointLightCount;
        for (unsigned int i = 0; i < shadowNum; ++i){
            pointLightShadowFBOs[i].bind();
            pointLightShadowFBOs[i].clear(GL_DEPTH_BUFFER_BIT, glm::vec3(1.0f));
            currentScene->drawPointLightShadow(pointShadowShader,i, pointLightShadowFBOs[i].depthBuffer);
        }
        
        return true;
    }

    void RenderManager::postProcess()
    {
        pingPongFBO.bind();
        pingPongFBO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::vec3(0.0f));
        if( sceneCamera->blurAmount > 0){
            //Filtering pixel rgb values > 1.0
            highPassFilterShader.use();
            canvas.draw(simpleFBO.texColorBuffer);
        }

        gaussianBlurShader.use();
        for (int i = 0; i < sceneCamera->blurAmount; ++i){
            //Horizontal pass
            glBindFramebuffer(GL_FRAMEBUFFER, simpleFBO.frameBufferID);
            glDrawBuffer(GL_COLOR_ATTACHMENT1);
            gaussianBlurShader.setBool("horizontal", true);
            canvas.draw(pingPongFBO.texColorBuffer);

            //Vertical pass
            glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO.frameBufferID);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            gaussianBlurShader.setBool("horizontal", false);
            canvas.draw(simpleFBO.blurHighEnd);
        }

        screen->bind();
        screenSpaceShader.use();

        screenSpaceShader.setFloat("exposure", sceneCamera->exposure);
        screenSpaceShader.setInt("screenTexture", 0);
        screenSpaceShader.setInt("bloomBlur", 1);
        screenSpaceShader.setInt("computeTexture", 2);

        canvas.draw(simpleFBO.texColorBuffer, pingPongFBO.texColorBuffer);
    }
}


