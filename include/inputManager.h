#pragma once
#include <SDL2/include/SDL_events.h>

namespace congb
{
    class InputManager
    {
    public:
        InputManager();
        ~InputManager();

        bool startUp();
        void shutDown();

        void processInput(bool &done, unsigned int deltaT);

    private:
        
        void handleEvent(SDL_Event* event, bool &done, unsigned int deltaT); 
    };
}
