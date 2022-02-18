#pragma once
#include <SDL.h>
#include "KTypes.h"
#include "MathUtils.h"

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
	static Color CHARCOAL = Color(30, 30, 30, 255);

	static Color RED = Color(255, 0, 0, 255);
	static Color GREEN = Color(0, 255, 0, 255);
	static Color BLUE = Color(0, 0, 255, 255);
	static Color YELLOW = Color(255, 255, 0, 255);
	static Color CYAN = Color(0, 255, 255, 255);
	static Color MAGENTA = Color(255, 0, 255, 255);
}

static Color InterpolateColor(Color color1, Color color2, float interpolation)
{
	interpolation = MathUtils::Clamp(interpolation, 0.f, 1.f);

	int r = color1.red * (1.f - interpolation) + color2.red * interpolation;
	int g = color1.green * (1.f - interpolation) + color2.green * interpolation;
	int b = color1.blue * (1.f - interpolation) + color2.blue * interpolation;
	int a = color1.alpha * (1.f - interpolation) + color2.alpha * interpolation;

	return Color(r, g, b, a);
}


inline std::ostream& operator<<(std::ostream& os, const Color& col)
{
	os << "[" << (int)col.red << " " << (int)col.green << " " << (int)col.blue << "]";
	return os;
}