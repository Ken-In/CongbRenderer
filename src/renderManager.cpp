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
        return true;
    }

    void RenderManager::shutDown()
    {
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


