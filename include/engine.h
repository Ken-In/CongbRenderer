#pragma once
#include "displayManager.h"

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
		DisplayManager gDisplayManager;
	};
}
