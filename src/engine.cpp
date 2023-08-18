#include "engine.h"

#include <stdio.h>

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

		unsigned int deltaTime = SDL_GetTicks() - startTime;
		printf("(Engine load time: %ums)\n", deltaTime);
		return true;
	}

	void Engine::shutdownEngine()
	{
		printf("ShutdownEngine\n");
	}
	void Engine::run()
	{
		printf("Engine Running...\n");
	}
}