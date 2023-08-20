#pragma once
#include "camera.h"
#include "displayManager.h"
#include "sceneManager.h"
#include "shader.h"

namespace congb
{
    class RenderManager
    {
    public:
        RenderManager();
        ~RenderManager();

        bool startUp(DisplayManager& displayManager, SceneManager& sceneManager);
        void shutDown();

        void render();

    private:
        bool initFBOs();
        bool loadShaders();
        bool preProcess();
        bool postProcess();

        Shader helloTriangleShader;

        Camera  *sceneCamera;
        DisplayManager  *screen;
        SceneManager    *sceneLocator;

    };
}
