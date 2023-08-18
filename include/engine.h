#pragma once
#include "displayManager.h"

namespace congb
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		bool InitEngine();
		void ShutdownEngine();
		void Run();

	private:
		displayManager gDisplayManager;
	};
}
