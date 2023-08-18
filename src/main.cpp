#include <cstdio>

#include "engine.h"

using namespace congb;

int main(int argc, char* argv[])
{
	Engine engine;
	if(engine.initEngine())
	{
		engine.run();
	}
	else
	{
		printf("Failed to initialize engine.\n");
	}
	
	engine.shutdownEngine();

	return 0;
}