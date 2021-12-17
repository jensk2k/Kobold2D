#include "Core.h"
#include <iostream>


int main(int argc, char** argv)
{
	Core core;
	core.Init();

	int targetFrameRate = core.GetDisplayRefreshRate() + 1;

	Uint32 frameStart = 0;
	Uint32 frameTime = 0;
	float frameDelay = 1000.f / targetFrameRate;
	std::cout << "frame delay is " << frameDelay << " ms" << std::endl;


	Uint32 time;
	while (core.IsRunning())
	{
		time = SDL_GetTicks();
		frameStart = SDL_GetTicks();

		core.HandleEvents();
		core.Update();
		core.Render();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
			SDL_Delay((Uint32)(frameDelay - frameTime));
	}

	core.Clean();

	return 0;
}