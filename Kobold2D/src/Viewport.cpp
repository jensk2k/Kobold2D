#include "Viewport.h"
#include "GameState.h"

Vec2i Viewport::WorldToScreenSpace(Vec2f worldPos) const
{
	Vec2f worldPosRelativeToVP = worldPos - position;

	int windowWidth = gameState.GetWindowWidth();
	int windowHeight = gameState.GetWindowHeight();

	Vec2i screenSpacePos;
	screenSpacePos.x = windowWidth / 2 + static_cast<int>(worldPosRelativeToVP.x * scale);
	screenSpacePos.y = windowHeight / 2 - static_cast<int>(worldPosRelativeToVP.y * scale);

	return screenSpacePos;
}

Vec2f Viewport::ScreenToWorldPosition(Vec2i screenPos) const
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
	Vec2f topLeftWS = ScreenToWorldPosition(Vec2i());
	Vec2f bottomRightWS = ScreenToWorldPosition(Vec2i(windowWidth, windowHeight));

	float left = topLeftWS.x;
	float right = bottomRightWS.x;
	float top = topLeftWS.y;
	float bottom = bottomRightWS.y;

	Color drawGrey(100, 100, 100);

	for (float x = left; x < right; x += scale)
	{
		Vec2f lineA_WS(x, top);
		Vec2f lineB_WS(x, bottom);
		gameState.DrawLine(WorldToScreenSpace(lineA_WS), WorldToScreenSpace(lineB_WS), drawGrey);
	}

	for (float y = bottom; y < top; y += scale)
	{
		Vec2f lineA_WS(left, y);
		Vec2f lineB_WS(right, y);
		gameState.DrawLine(WorldToScreenSpace(lineA_WS), WorldToScreenSpace(lineB_WS), drawGrey);
	}

	gameState.DrawLine(WorldToScreenSpace(Vec2f(-1.f, 0.f)), WorldToScreenSpace(Vec2f(1.f, 0.f)), Colors::WHITE);
	gameState.DrawLine(WorldToScreenSpace(Vec2f(0.f, -1.f)), WorldToScreenSpace(Vec2f(0.f, 1.f)), Colors::WHITE);
}

void Viewport::DrawRectangle(Vec2f position, float width, float height, Color color) const
{
	gameState.DrawRectangle(WorldToScreenSpace(position), width * scale, height * scale, color);
}

void Viewport::DrawRectangleSolid(Vec2f position, float width, float height, Color color) const
{
	gameState.DrawRectangleSolid(WorldToScreenSpace(position), width * scale, height * -scale, color);
}

void Viewport::DrawCircle(Vec2f c, float radius, Color color)
{
	gameState.DrawCircle(WorldToScreenSpace(c), radius * scale, color);
}

void Viewport::DrawCircleSolid(Vec2f c, float radius, Color color)
{
	gameState.DrawCircleSolid(WorldToScreenSpace(c), radius * scale, color);
}

void Viewport::DrawLine(Vec2f v1, Vec2f v2, Color color)
{
	gameState.DrawLine(WorldToScreenSpace(v1), WorldToScreenSpace(v2), color);
}

void Viewport::DrawArrow(Vec2f origin, Vec2f direction, float scale, Color color)
{
	Vec2f left(-direction.y, direction.x);
	Vec2f right(direction.y, -direction.x);

	Vec2i originSS = WorldToScreenSpace(origin);
	Vec2i targetSS = WorldToScreenSpace(origin + direction * scale);

	Vec2i rightChev = WorldToScreenSpace(origin + direction * scale + right * 0.2f * scale - direction * 0.2f * scale);
	Vec2i leftChev = WorldToScreenSpace(origin + direction * scale + left * 0.2f * scale - direction * 0.2f * scale);

	gameState.DrawLine(originSS, targetSS, color);
	gameState.DrawLine(targetSS, rightChev, color);
	gameState.DrawLine(targetSS, leftChev, color);
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