#pragma once
#include <map>
#include <SDL.h>

enum class Keys
{
	INVALID,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	SPACE,
	W,
	A,
	S,
	D,
};

class Input
{
public:
	Input();
	Keys GetKeyMapping(SDL_Keycode keycode) const;

private:
	std::map<SDL_Keycode, Keys> rawKeyMap;
};

