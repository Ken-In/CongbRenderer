#include "inputManager.h"

namespace congb
{
    InputManager::InputManager()
    {
    }

    InputManager::~InputManager()
    {
    }

    bool InputManager::startUp()
    {
        return true;
    }

    void InputManager::shutDown()
    {
    }

    void InputManager::processInput(bool& done, unsigned deltaTime)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if(done)
            {
                return;
            }
            else
            {
                handleEvent(&event, done, deltaTime);
            }
        }
    }

    void InputManager::handleEvent(SDL_Event* event, bool& done, unsigned deltaTime)
    {
        bool isDown = event->type == SDL_KEYDOWN;
        bool wasDown = event->type == SDL_KEYUP;

        if(isDown || wasDown)
        {
            switch (event->key.keysym.sym)
            {
            case SDLK_ESCAPE:
                if(isDown)
                    done = true;
                return;
            }
        }
    }
}
