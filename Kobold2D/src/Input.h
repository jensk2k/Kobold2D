#pragma once
#include <map>
#include <SDL.h>

enum class Keys
{
	INVALID,
	UP,	DOWN, LEFT, RIGHT,
	SPACE,
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, ZERO,
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	F11
};

enum class MouseButtons
{
	INVALID, LEFT, MIDDLE, RIGHT, ONE, TWO
};

class Input
{
public:
	Input();
	Keys GetKeyMapping(SDL_Keycode keycode) const;

private:
	std::map<SDL_Keycode, Keys> rawKeyMap;
};

