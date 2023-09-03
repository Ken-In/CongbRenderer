#pragma once

#include <SDL.h>

namespace congb
{
    class DisplayManager
    {
    public:
        constexpr  static int SCREEN_WIDTH  = 1280;
        constexpr static int SCREEN_HEIGHT = 720;
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
        bool createImGuiContext();


        SDL_Window*      mWindow;
        SDL_GLContext    mContext;
    };
}
