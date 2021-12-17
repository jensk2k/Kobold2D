#pragma once
#include <SDL.h>

struct Color
{
	Color() : red(0), green(0), blue(0), alpha(0) {}
	Color(Uint8 r, Uint8 g, Uint8 b) : red(r), green(g), blue(b), alpha(255) {}
	Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : red(r), green(g), blue(b), alpha(a) {}
	Uint8 red, green, blue, alpha;
};

namespace Colors
{
	static Color WHITE = Color(255, 255, 255, 255);
	static Color BLACK = Color(0, 0, 0, 255);
	static Color CHARCOAL = Color(51, 51, 51, 255);

	static Color RED = Color(255, 0, 0, 255);
	static Color GREEN = Color(0, 255, 0, 255);
	static Color BLUE = Color(0, 0, 255, 255);
	static Color YELLOW = Color(255, 255, 0, 255);
	static Color CYAN = Color(0, 255, 255, 255);
	static Color MAGENTA = Color(255, 0, 255, 255);
}