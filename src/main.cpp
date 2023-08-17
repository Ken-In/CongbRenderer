#include "engine.h"

using namespace congb;

int main(int argc, char* argv[])
{
	Engine engine;
	engine.InitEngine();
	engine.Run();
	engine.ShutdownEngine();

	return 0;
}