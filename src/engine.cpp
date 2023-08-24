#include "engine.h"

#include <cstdio>

namespace congb{
	Engine::Engine() {}
	Engine::~Engine() {}

	bool Engine::initEngine()
	{
		printf("InitEngine\n");
		unsigned int startTime = SDL_GetTicks();

		if(!gDisplayManager.startUp())
		{
			printf("Failed to initialize displayManager.\n");
			return false;
		}
		if(!gSceneManager.startUp())
		{
			printf("Failed to initialize sceneManager.\n");
			return false;
		}
		if(!gRenderManager.startUp(gDisplayManager, gSceneManager))
		{
			printf("Failed to initialize renderManager.\n");
			return false;
		}
		if(!gInputManager.startUp(gSceneManager))
		{
			printf("Failed to initialize inputManager.\n");
			return false;
		}
		unsigned int deltaTime = SDL_GetTicks() - startTime;
		printf("(Engine load time: %ums)\n", deltaTime);
		return true;
	}

	void Engine::shutdownEngine()
	{
		printf("Shutdown engine\n");
	}
	void Engine::run()
	{
		printf("Engine running...\n");
		bool done = false;
		unsigned int deltaTime = SDL_GetTicks();
		unsigned int count = 0;
		unsigned int start = 0;
		unsigned int total = 0;
		while(!done)
		{
			start = SDL_GetTicks();
			count++;

			gInputManager.processInput(done, deltaTime);

			gSceneManager.update(deltaTime);
			
			gRenderManager.render();
			
			deltaTime = SDL_GetTicks() - start;
			total += deltaTime;
		}

		printf("\nPerformance Stats:\n------------------\n");
		printf("Average frame time over %2.1d frames:%2.fms.\n\n", count, total/(float)count);
		printf("Stop running engine...\n");
	}
}