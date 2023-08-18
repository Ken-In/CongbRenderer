﻿#pragma once

#include <SDL.h>

namespace congb
{
    class DisplayManager
    {
    public:
        constexpr  static int SCREEN_WIDTH  = 1920;
        constexpr static int SCREEN_HEIGHT = 1080;
        constexpr static float SCREEN_ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

        const char* GLSL_VERSION = "#version 450";

        DisplayManager();
        ~DisplayManager();
        
        bool startUp();
        void shutDown();

        void bind();
        void swapDisplayBuffer();
        
    private:
        bool startSDL();
        bool startOpenGL();
        bool createWindow();
        bool createGLContext();

        SDL_Window*      mWindow;
        SDL_GLContext    mContext;
    };
}