#include "TrailMap.h"
#include "MathUtils.h"
#include "GameState.h"

static Color InterpolateColor(Color color1, Color color2, float interpolation)
{
	interpolation = MathUtils::Clamp(interpolation, 0.f, 1.f);

	int r = color1.red * (1.f - interpolation) + color2.red * interpolation;
	int g = color1.green * (1.f - interpolation) + color2.green * interpolation;
	int b = color1.blue * (1.f - interpolation) + color2.blue * interpolation;

	return Color(r, g, b);
}

static Color GetMapCoordColor(const Map2D<Color>& map, Vec2i coord)
{
	if (coord.x < 0 || coord.y < 0 || coord.x >= map.m_width || coord.y >= map.m_height)
		return Colors::BLACK;
	return map(coord.x, coord.y);
}

static void AccumulateColor(Color color, int& r, int& g, int& b)
{
	r += color.red;
	g += color.green;
	b += color.blue;
}

static Color GetAvgOfNeighbors(const Map2D<Color>& map, Vec2i coord)
{
	int r = 0;
	int g = 0;
	int b = 0;

	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, -1)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, -1)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, -1)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, 0)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, 0)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, 0)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, 1)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, 1)), r, g, b);
	AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, 1)), r, g, b);

	return Color(r / 9, g / 9, b / 9);
}

void TrailMap::Update(float propagationTime)
{
	const Map2D<Color>& inactiveMap = !isMap1Active ? map1 : map2;
	Map2D<Color>& activeMap = isMap1Active ? map1 : map2;

	for (int y = 0; y < activeMap.m_height; y++)
	{
		for (int x = 0; x < activeMap.m_width; x++)
		{
			Color curColor = inactiveMap(x, y);
			Color avgNeighbors = GetAvgOfNeighbors(inactiveMap, Vec2i(x, y));
			activeMap(x, y) = InterpolateColor(curColor, avgNeighbors, gameState.GetDeltaTime() * propagationTime);
			//activeMap(x, y) = avgNeighbors;
		}
	}
}

void TrailMap::Trail(Vec2i position, int radius, Color color)
{
	Map2D<Color>& activeMap = isMap1Active ? map1 : map2;

	int convFactor = gameState.GetWindowWidth() / map1.m_width;
	Vec2i posMS = position / convFactor;
	float radiusMS = (float)radius / (float)convFactor;

	if (radiusMS < 1.f)
	{
		color = Color(color.red / radiusMS, color.green / radiusMS, color.blue / radiusMS);
	}


	for (int y = 0; y < activeMap.m_height; y++)
	{
		for (int x = 0; x < activeMap.m_width; x++)
		{
			Vec2f posA((float)x, (float)y);
			Vec2f posB((float)posMS.x, (float)posMS.y);

			if ((posA - posB).Length2() < MathUtils::Square(radiusMS))
				activeMap(x, y) = color;
		}
	}
}

void TrailMap::Render()
{
	Map2D<Color>& activeMap = isMap1Active ? map1 : map2;

	gameState.RenderPixelMapToTexture(activeMap, texture);
	gameState.DrawTexture(texture, Vec2i(), gameState.GetWindowWidth(), gameState.GetWindowHeight());

	isMap1Active = !isMap1Active;
}
