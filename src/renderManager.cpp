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

        // todo: FBO
        
        printf("Loading Shaders...\n");
        if(!loadShaders())
        {
            printf("Shaders failed to be initialized correctly.\n");
            return false;
        }

        // todo: SSBO

        // todo: Preprocessing


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
        
        glDepthFunc(GL_LEQUAL);
        glDepthMask(false);
        currentScene->drawFullScene(simpleShader);
        
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("OpenGL error：%d\n", error);
        }
        screen->swapDisplayBuffer();
    }

    bool RenderManager::initFBOs()
    {
        return true;
    }

    bool RenderManager::loadShaders()
    {
        bool success = true;
        success &= helloTriangleShader.setup("1_helloTriangle.vert", "1_helloTriangle.frag", "");
        success &= simpleShader.setup("2_simpleShader.vert", "2_simpleShader.frag", "");

        if(!success)
        {
            printf("Error loading Shaders!\n");
            return false;
        }
        
        return true;
    }

    bool RenderManager::preProcess()
    {
        return true;
    }

    void RenderManager::postProcess()
    {
    }
}


