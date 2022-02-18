#include "TrailMap.h"
#include "MathUtils.h"
#include "GameState.h"

//static Color InterpolateColor(Color color1, Color color2, float interpolation)
//{
//	interpolation = MathUtils::Clamp(interpolation, 0.f, 1.f);
//
//	int r = color1.red * (1.f - interpolation) + color2.red * interpolation;
//	int g = color1.green * (1.f - interpolation) + color2.green * interpolation;
//	int b = color1.blue * (1.f - interpolation) + color2.blue * interpolation;
//	int a = color1.alpha * (1.f - interpolation) + color2.alpha * interpolation;
//
//	return Color(r, g, b, a);
//}

static Color GetMapCoordColor(const Map2D<Color>& map, Vec2i coord)
{
	Color clearColor(0, 0, 0, 0);

	if (coord.x < 0 || coord.y < 0 || coord.x >= map.m_width || coord.y >= map.m_height)
		return clearColor;
	return map(coord.x, coord.y);
}

static void AccumulateColor(Color color, int& r, int& g, int& b, int& a)
{
	r += color.red;
	g += color.green;
	b += color.blue;
	a += color.alpha;
}

static Color GetAvgOfNeighbors(const Map2D<Color>& map, Vec2i coord)
{
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	for (int y = -1; y < 1; y++)
		for (int x = -1; x < 1; x++)
			AccumulateColor(GetMapCoordColor(map, coord + Vec2i(x, y)), r, g, b, a);

	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, -1)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, -1)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, -1)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, 0)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, 0)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, 0)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(-1, 1)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(0, 1)), r, g, b);
	//AccumulateColor(GetMapCoordColor(map, coord + Vec2i(1, 1)), r, g, b);

	return Color(r / 9, g / 9, b / 9, a / 9);
}

TrailMap::TrailMap(GameState& gameState, int width, int height) :
	gameState(gameState),
	map1(width, height),
	map2(width, height) 
{
	Color clearColor(0, 0, 0, 0);
	for (int y = 0; y < map1.m_height; y++)
	{
		for (int x = 0; x < map1.m_width; x++)
		{
			map1(x, y) = clearColor;
		}
	}
	for (int y = 0; y < map2.m_height; y++)
	{
		for (int x = 0; x < map2.m_width; x++)
		{
			map2(x, y) = clearColor;
		}
	}
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
		radiusMS = 1.f;
	//if (radiusMS < 1.f)
	//{
	//	color = Color(color.red / radiusMS, color.green / radiusMS, color.blue / radiusMS);
	//}


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
