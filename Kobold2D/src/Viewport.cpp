#include "Viewport.h"
#include "GameState.h"


Vec2f Viewport::WorldToScreenSpace(Vec2f worldPos) const
{
	Vec2f worldPosRelativeToVP = worldPos - position;

	int windowWidth = gameState.GetWindowWidth();
	int windowHeight = gameState.GetWindowHeight();

	Vec2f screenSpacePos;
	screenSpacePos.x = windowWidth / 2.f + worldPosRelativeToVP.x * scale;
	screenSpacePos.y = windowHeight / 2.f - worldPosRelativeToVP.y * scale;

	return screenSpacePos;
}

Vec2f Viewport::ScreenToWorldPosition(Vec2f screenPos) const
{
	int windowWidth = gameState.GetWindowWidth();
	int windowHeight = gameState.GetWindowHeight();

	Vec2f wsPos;
	Vec2f wsPosTemp;
	wsPosTemp.x = screenPos.x - (windowWidth / 2.f);
	wsPosTemp.y = -(screenPos.y - (windowHeight / 2.f));

	wsPos.x = wsPosTemp.x / scale;
	wsPos.y = wsPosTemp.y / scale;

	wsPos += position;

	return wsPos;
}

void Viewport::DrawGrid(GameState& gameState, int scale)
{
	if (scale < 1)
		scale = 1;

	int windowWidth = gameState.GetWindowWidth();
	int windowHeight = gameState.GetWindowHeight();
	Vec2f topLeftWS = ScreenToWorldPosition(Vec2f());
	Vec2f bottomRightWS = ScreenToWorldPosition(Vec2f(windowWidth, windowHeight));

	int left = topLeftWS.x;
	int right = bottomRightWS.x;
	int top = topLeftWS.y;
	int bottom = bottomRightWS.y;

	Color drawGrey(100, 100, 100);

	for (int x = left; x < right; x += scale)
	{
		Vec2f lineA_WS(x, top);
		Vec2f lineB_WS(x, bottom);
		gameState.DrawLine(WorldToScreenSpace(lineA_WS), WorldToScreenSpace(lineB_WS), drawGrey);
	}

	for (int y = bottom; y < top; y += scale)
	{
		Vec2f lineA_WS(left, y);
		Vec2f lineB_WS(right, y);
		gameState.DrawLine(WorldToScreenSpace(lineA_WS), WorldToScreenSpace(lineB_WS), drawGrey);
	}

	gameState.DrawLine(WorldToScreenSpace(Vec2f(-1.f, 0.f)), WorldToScreenSpace(Vec2f(1.f, 0.f)), Colors::WHITE);
	gameState.DrawLine(WorldToScreenSpace(Vec2f(0.f, -1.f)), WorldToScreenSpace(Vec2f(0.f, 1.f)), Colors::WHITE);
}

void ViewPortControl::HandleKeyDown(Keys key)
{
	if (key == Keys::UP)
		up = true;
	if (key == Keys::DOWN)
		down = true;
	if (key == Keys::LEFT)
		left = true;
	if (key == Keys::RIGHT)
		right = true;
}

void ViewPortControl::HandleKeyUp(Keys key)
{
	if (key == Keys::UP)
		up = false;
	if (key == Keys::DOWN)
		down = false;
	if (key == Keys::LEFT)
		left = false;
	if (key == Keys::RIGHT)
		right = false;
}

void ViewPortControl::Update(Viewport& vp, float deltaTime)
{
	float moveSpeed = 50.f;
	if (up)
		vp.position.y += moveSpeed * deltaTime;
	if (down)
		vp.position.y -= moveSpeed * deltaTime;
	if (left)
		vp.position.x -= moveSpeed * deltaTime;
	if (right)
		vp.position.x += moveSpeed * deltaTime;
}