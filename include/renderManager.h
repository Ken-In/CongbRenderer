#pragma once
#include "camera.h"
#include "displayManager.h"
#include "framebuffer.h"
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
        bool initSSBOs();
        bool loadShaders();
        bool preProcess();
        void postProcess();

        Shader helloTriangleShader, simpleShader, skyboxShader, fillCubeMapShader,
               dirShadowShader, pointShadowShader,
               convolveCubeMapShader, preFilterSpecShader, integrateBRDFShader,
               highPassFilterShader, gaussianBlurShader, screenSpaceShader;

        ComputeShader buildAABBGridCompShader, cullLightsCompShader;
        
        Camera  *sceneCamera;
        Scene   *currentScene;
        DisplayManager  *screen;
        SceneManager    *sceneLocator;

        Quad canvas;

        // cluster settings
        const unsigned int gridSizeX = 16;
        const unsigned int gridSizeY = 9;
        const unsigned int gridSizeZ = 24;
        const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ; 
        unsigned int sizeX, sizeY;

        // light settings
        unsigned int numLights;
        const unsigned int maxLights = 5000;
        const unsigned int maxLightsPerTile = 100;

        // SSBO
        unsigned int AABBvolumeGridSSBO, screenToViewSSBO;
        unsigned int lightSSBO, lightIndexListSSBO, lightGridSSBO, lightIndexGlobalCountSSBO;

        ResolveBuffer simpleFBO;
        CaptureBuffer captureFBO;
        QuadHDRBuffer pingPongFBO;
        FrameBufferMultiSampled multiSampledFBO;
        DirShadowBuffer  dirShadowFBO;
        PointShadowBuffer* pointLightShadowFBOs;
    };
}
