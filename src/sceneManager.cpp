#include "sceneManager.h"

namespace congb
{
    SceneManager::SceneManager()
    {
    }

    SceneManager::~SceneManager()
    {
        delete currentScene;
    }

    bool SceneManager::startUp()
    {
        currentSceneID = "sponza";
        if(!loadScene(currentSceneID))
        {
            printf("Could not load default sponza scene. No models succesfully loaded!\n");
            return false;
        }
        return true;
    }

    void SceneManager::shutDown()
    {
        delete currentScene;
    }

    bool SceneManager::switchScene(std::string newSceneID)
    {
        if(newSceneID != currentSceneID)
        {
            currentSceneID = newSceneID;
            delete currentScene;
            return loadScene(newSceneID);
        }
        else
        {
            printf("Selected already loaded scene.\n");
            return true;
        }
    }

    void SceneManager::update(unsigned deltaTime)
    {
        currentScene->update(deltaTime);
    }

    Scene* SceneManager::getCurrentScene()
    {
        return currentScene;
    }

    bool SceneManager::loadScene(std::string sceneID)
    {
        currentScene = new Scene(sceneID);
        return !currentScene->loadingError;
    }
}
