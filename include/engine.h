#pragma once

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
	};
}
