#pragma once
#include <SDL.h>
#include "KTypes.h"

struct Color
{
	Color() : red(0), green(0), blue(0), alpha(0) {}
	Color(uint8 r, uint8 g, uint8 b) : red(r), green(g), blue(b), alpha(255) {}
	Color(uint8 r, uint8 g, uint8 b, uint8 a) : red(r), green(g), blue(b), alpha(a) {}
	uint8 red, green, blue, alpha;
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

inline std::ostream& operator<<(std::ostream& os, const Color& col)
{
	os << "[" << (int)col.red << " " << (int)col.green << " " << (int)col.blue << "]";
	return os;
}