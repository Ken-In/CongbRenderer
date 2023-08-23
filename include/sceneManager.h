#pragma once
#include <string>

#include "scene.h"

namespace congb
{
    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        bool startUp();
        void shutDown();

        bool switchScene(std::string newSceneID);

        void update(unsigned int deltaTime);

        Scene* getCurrentScene();

    private:
        bool loadScene(std::string sceneID);

        std::string currentSceneID;
        Scene* currentScene;
    };
}

