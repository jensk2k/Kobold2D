#include "Input.h"
#include <SDL.h>

Input::Input() :
	rawKeyMap
	{
		{ SDLK_UP, Keys::UP },
		{ SDLK_DOWN, Keys::DOWN },
		{ SDLK_LEFT, Keys::LEFT },
		{ SDLK_RIGHT, Keys::RIGHT },
		{ SDLK_SPACE, Keys::SPACE },
		{ SDLK_w, Keys::W },
		{ SDLK_a, Keys::A },
		{ SDLK_s, Keys::S },
		{ SDLK_d, Keys::D },
	}

{
	//rawKeyMap.insert(pair<int32_t, Keys>(1, 40) SDLK_SPACE, Keys::SPACE);
}

Keys Input::GetKeyMapping(SDL_Keycode keycode) const
{
	std::map<SDL_Keycode, Keys>::const_iterator it = rawKeyMap.find(keycode);

	if (it != rawKeyMap.end())
		return it->second;
	else
		return Keys::INVALID;
}
