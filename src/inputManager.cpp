#include "inputManager.h"

namespace congb
{
    InputManager::InputManager()
    {
    }

    InputManager::~InputManager()
    {
    }

    bool InputManager::startUp(SceneManager& sceneManager)
    {
        sceneController = &sceneManager;
        sceneCamera = sceneController->getCurrentScene()->getCurrentCamera();
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
                {
                    done = true;
                }
                return;
            case SDLK_r:
                if(isDown)
                {
                    sceneCamera->resetCamera();
                }
                break;
            case SDLK_w:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('w');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('w');
                }
                break;
            case SDLK_s:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('s');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('s');
                }
                break;
            case SDLK_a:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('a');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('a');
                }
                break;
            case SDLK_d:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('d');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('d');
                }
                break;
            case SDLK_q:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('q');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('q');
                }
                break;
            case SDLK_e:
                if(isDown)
                {
                    sceneCamera->activeMoveStates.insert('e');
                }
                if(wasDown)
                {
                    sceneCamera->activeMoveStates.erase('e');
                }
                break;
            default:
                break;
            }
        }
        else if(event->type == SDL_MOUSEMOTION)
        {
            if(event->motion.state & SDL_BUTTON_LMASK)
            {
                SDL_SetRelativeMouseMode(SDL_TRUE);

                float sens = sceneCamera->mouseSens;
                float xOffset = (float)event->motion.xrel * sens;
                float yOffset = -(float)event->motion.yrel * sens;

                sceneCamera->yaw    = glm::mod(sceneCamera->yaw + xOffset, 360.0f);
                sceneCamera->pitch += yOffset;

                if(sceneCamera->pitch > 89.0f)
                {
                    sceneCamera->pitch = 89.0f;
                }
                else if(sceneCamera->pitch < -89.0f)
                {
                    sceneCamera->pitch = -89.0f;
                }
            }
            else
            {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }
    }
}
