#pragma once
#include "Vector2.h"
#include "Input.h"
#include "Color.h"

class GameState;

struct Viewport
{
	Viewport(GameState& gameState, float scale) 
		: gameState(gameState) 
		, scale(scale)
	{}
	Vec2i WorldToScreenSpace(Vec2f worldPos) const;
	Vec2f ScreenToWorldPosition(Vec2i screenPos) const;
	void DrawGrid(GameState& gameState, int scale);
	void DrawRectangle(Vec2f position, float width, float height, Color color) const;
	void DrawRectangleSolid(Vec2f position, float width, float height, Color color) const;
	void DrawCircle(Vec2f c, float radius, Color color);
	void DrawCircleSolid(Vec2f c, float radius, Color color);
	void DrawLine(Vec2f v1, Vec2f v2, Color color);
	void DrawArrow(Vec2f origin, Vec2f direction, float scale, Color color);

	Vec2f position;
	float scale/* = 12.8f*/;
private:
	GameState& gameState;
};

class ViewPortControl
{
public:
	void HandleKeyDown(Keys key);
	void HandleKeyUp(Keys key);
	void Update(Viewport& vp, float deltaTime);

private:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};