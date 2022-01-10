#pragma once
#include "Map2D.h"
#include "Vector2.h"
#include "Texture.h"
#include "Color.h"

class GameState;

class TrailMap
{
public:
	TrailMap(GameState& gameState, int width, int height) :
		gameState(gameState),
		map1(width, height),
		map2(width, height) {}

	void Update(float propagationTime);
	void Trail(Vec2i position, int radius, Color color);
	void Render();
private:
	GameState& gameState;
	Texture texture;
	Map2D<Color> map1;
	Map2D<Color> map2;

	bool isMap1Active = true;
};

