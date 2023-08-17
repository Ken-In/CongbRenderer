#include "engine.h"

#include <stdio.h>

namespace congb{
	Engine::Engine() {}
	Engine::~Engine() {}

	bool Engine::InitEngine()
	{
		printf("InitEngine\n");
		return true;
	}

	void Engine::ShutdownEngine()
	{
		printf("ShutdownEngine\n");
	}
	void Engine::Run()
	{
		printf("Engine Running...\n");
	}
}