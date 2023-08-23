#pragma once
#include "camera.h"
#include "displayManager.h"
#include "meshPrimitives.h"
#include "scene.h"
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
        void postProcess();

        Shader helloTriangleShader, simpleShader;

        Camera  *sceneCamera;
        Scene   *currentScene;
        DisplayManager  *screen;
        SceneManager    *sceneLocator;
        //unsigned int    VAO, VBO, EBO;
    };
}
