#pragma once
#include "displayManager.h"
#include "inputManager.h"
#include "renderManager.h"
#include "sceneManager.h"

namespace congb
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		bool initEngine();
		void shutdownEngine();
		void run();

	private:
		DisplayManager	gDisplayManager;
		InputManager	gInputManager;
		SceneManager	gSceneManager;
		RenderManager	gRenderManager;
	};
}
