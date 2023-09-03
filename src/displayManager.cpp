#include "displayManager.h"

#include <cstdio>
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

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

        if( !createImGuiContext()){
            return false;
        }
        
        return true;
    }

    void DisplayManager::shutDown()
    {
        // ui
        ImGui::EndFrame();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        // sdl
        SDL_GL_DeleteContext(mContext); 

        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;

        SDL_Quit();
    }

    void DisplayManager::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DisplayManager::swapDisplayBuffer()
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(mWindow);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();
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

        printf("Vendor:     %s\n", glGetString(GL_VENDOR));
        printf("Renderer:   %s\n", glGetString(GL_RENDERER));
        printf("Version:    %s\n", glGetString(GL_VERSION));

        // 交换buffer 1 = 垂直同步
        SDL_GL_SetSwapInterval(1);
        glEnable(GL_CULL_FACE);
        // 多重采样
        glEnable(GL_MULTISAMPLE);
        // gamma 矫正
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        int w, h;
        SDL_GetWindowSize(mWindow, &w, &h);
        glViewport(0, 0, w, h);
        return true;
    }

    bool DisplayManager::createImGuiContext()
    {
        ImGuiContext * mGuiContext = ImGui::CreateContext();
        if( mGuiContext == nullptr){
            printf("Could not load IMGUI context!\n");
            return false;
        }

        //Init and configure for OpenGL and SDL
        ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);

        //Imgui first frame setup
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();
        return true;
    }
}
