#pragma once
#include "Vector2.h"
#include "Input.h"

class GameState;

struct Viewport
{
	Viewport(GameState& gameState) : gameState(gameState) {}
	Vec2f WorldToScreenSpace(Vec2f worldPos) const;
	Vec2f ScreenToWorldPosition(Vec2f screenPos) const;
	void DrawGrid(GameState& gameState, int scale);

	Vec2f position;
	float scale = 12.8f;
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