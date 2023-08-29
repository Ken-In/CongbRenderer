#include "renderManager.h"

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
        pointLightShadowFBOs = new PointShadowBuffer[numLights];
        for(unsigned int i = 0; i < numLights; ++i ){
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
        
        //Setting up lights buffer that contains all the point lights in the scene
        {
            glGenBuffers(1, &lightSSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

            GLint bufMask = GL_READ_WRITE;

            struct GPULight *lights = (struct GPULight *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);
            PointLight *light;
            for(unsigned int i = 0; i < numLights; ++i ){
                //Fetching the light from the current scene
                light = currentScene->getPointLight(i);
                lights[i].position  = glm::vec4(light->position, 1.0f);
                lights[i].color     = glm::vec4(light->color, 1.0f);
                lights[i].enabled   = 1; 
                lights[i].intensity = 1.0f;
                lights[i].range     = 65.0f;
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
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

        // fill skybox
        captureFBO.bind();
        if(currentScene->mainSkybox.isHDR)
        {
            currentScene->mainSkybox.fillCubeMapWithTexture(fillCubeMapShader);
        }

        // draw point light shadow maps
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        for (unsigned int i = 0; i < currentScene->pointLightCount; ++i){
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


