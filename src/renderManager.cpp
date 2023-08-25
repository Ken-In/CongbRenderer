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

        // todo: SSBO

        // todo: Preprocessing
        printf("Preprocessing...\n");
        if (!preProcess()){
            printf("SSBO's failed to be initialized correctly.\n");
            return false;
        }

        /*float vertices[] = 
        {
            0.0f, 0.5f, 0.0f,   
            -0.5f, -0.5f, 0.0f,  
            0.5f, -0.5f, 0.0f, 
        };

        unsigned int indices[] = {
            0, 1, 2, 
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);*/
        
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
        /*glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        helloTriangleShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);*/

        glViewport(0, 0, screen->SCREEN_WIDTH, screen->SCREEN_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // depth test setting
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        currentScene->drawFullScene(simpleShader, skyboxShader);
        
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("OpenGL error：%d\n", error);
        }
        screen->swapDisplayBuffer();
    }

    bool RenderManager::initFBOs()
    {
        bool stillValid = true;
        int skyboxRes = currentScene->mainSkybox.resolution;
        
        stillValid &= captureFBO.setupFrameBuffer(skyboxRes, skyboxRes);
        
        return stillValid;
    }

    bool RenderManager::loadShaders()
    {
        bool success = true;
        success &= helloTriangleShader.setup("1_helloTriangle.vert", "1_helloTriangle.frag", "");
        success &= simpleShader.setup("2_simpleShader.vert", "2_simpleShader.frag", "");
        success &= skyboxShader.setup("3_skyboxShader.vert", "3_skyboxShader.frag", "");
        success &= fillCubeMapShader.setup("3_buildCubeMapShader.vert", "3_buildCubeMapShader.frag");
        
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
        currentScene->mainSkybox.fillCubeMapWithTexture(fillCubeMapShader);
        
        
        return true;
    }

    void RenderManager::postProcess()
    {
    }
}


