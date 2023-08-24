#pragma once
#include <SDL2/include/SDL_events.h>

#include "sceneManager.h"

namespace congb
{
    class InputManager
    {
    public:
        InputManager();
        ~InputManager();

        bool startUp(SceneManager& sceneManager);
        void shutDown();

        void processInput(bool &done, unsigned int deltaT);

    private:
        SceneManager* sceneController;
        Camera* sceneCamera;
        
        void handleEvent(SDL_Event* event, bool &done, unsigned int deltaT); 
    };
}
