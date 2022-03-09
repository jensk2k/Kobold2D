#include "Core.h"
#include <iostream>
#include <cmath>

int main(int argc, char** argv)
{
	Core core;
	core.Init();

	int targetFrameRate = core.GetDisplayRefreshRate();

	Uint32 frameStart = 0;
	Uint32 frameTime = 0;
	Uint32 frameDelay = round(1000.f / targetFrameRate);
	std::cout << "frame delay is " << frameDelay << " ms" << std::endl;

	while (core.IsRunning())
	{
		frameStart = SDL_GetTicks();

		core.HandleEvents();
		core.Update();
		core.Render();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
			SDL_Delay(frameDelay - frameTime);
	}

	core.Clean();

	return 0;
}