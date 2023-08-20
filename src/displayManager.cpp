#include "displayManager.h"

#include <cstdio>
#include <glad/glad.h>

namespace congb
{
    DisplayManager::DisplayManager(){}
    DisplayManager::~DisplayManager(){}

    bool DisplayManager::startUp()
    {
        if( !startSDL() ){
            return false;
        }

        if( !startOpenGL() ){
            return false;
        }

        if (!createWindow()){
            return false;
        }

        if( !createGLContext()){
            return false;
        }
        return true;
    }

    void DisplayManager::shutDown()
    {
    }

    void DisplayManager::bind()
    {
    }

    void DisplayManager::swapDisplayBuffer()
    {
    }

    bool DisplayManager::startSDL()
    {
        if( SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            printf("Failed to initialize SDL. Error: %s\n", SDL_GetError());
            return false;
        }
        return true;
    }

    bool DisplayManager::startOpenGL()
    {
        if(SDL_GL_LoadLibrary(nullptr) != 0)
        {
            printf("Failed to initialize OpenGL. Error: %s\n", SDL_GetError());
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

        return true;
    }

    bool DisplayManager::createWindow()
    {
        mWindow = SDL_CreateWindow("Congb Renderer",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
        if(mWindow == nullptr)
        {
            printf("Failed to create window. Error: %s\n", SDL_GetError());
            return false;
        }
        return true;
    }

    bool DisplayManager::createGLContext()
    {
        mContext = SDL_GL_CreateContext(mWindow);
        if(mContext == nullptr)
        {
            printf("Failed to create OpenGL context. Error: %s\n", SDL_GetError());
            return false;
        }

        if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
        {
            printf("GLAD could not load SDL Context.\n");
            return false;
        }

        printf("Vendor:     $s\n", glGetString(GL_VENDOR));
        printf("Renderer:   $s\n", glGetString(GL_RENDERER));
        printf("Version:    $s\n", glGetString(GL_VERSION));

        // 交换buffer 1 = 垂直同步
        SDL_GL_SetSwapInterval(1);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        int w, h;
        SDL_GetWindowSize(mWindow, &w, &h);
        glViewport(0, 0, w, h);
        return true;
    }
}
