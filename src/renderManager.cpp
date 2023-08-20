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
        
        printf("Loading Shaders...\n");
        if(!loadShaders())
        {
            printf("Shaders failed to be initialized correctly.\n");
            return false;
        }

        printf("Renderer Initialization complete.\n");
        return true;
    }

    void RenderManager::shutDown()
    {
        screen      = nullptr;
        sceneCamera = nullptr;
        sceneLocator= nullptr;
    }

    void RenderManager::render()
    {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
    }

    bool RenderManager::initFBOs()
    {
        return true;
    }

    bool RenderManager::loadShaders()
    {
        bool success = false;
        success &= helloTriangleShader.setup("1_helloTriangle.vert", "1_helloTriangle.frag", "");

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

    bool RenderManager::postProcess()
    {
        return true;
    }
}


