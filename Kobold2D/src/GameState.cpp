#include "GameState.h"
#include "Core.h"

void GameState::LoadTexture(const char* texturePath, Texture& texture, int width, int height)
{
	m_core.LoadTexture(texturePath, texture, width, height);
}

void GameState::DrawRectangle(Vec2f position, int width, int height, Color color) const
{
	m_core.DrawRectangle(position, width, height, color);
}

void GameState::DrawRectangleSolid(Vec2f position, int width, int height, Color color) const
{
	m_core.DrawRectangleSolid(position, width, height, color);
}

void GameState::DrawTexture(const Texture& texture, Vec2f position, int width, int height)
{
	m_core.DrawTexture(texture, position, width, height);
}

void GameState::DrawLine(Vec2f v1, Vec2f v2, Color color)
{
	m_core.DrawLine(v1, v2, color);
}

void GameState::DrawCircle(Vec2f c, float radius, Color color)
{
	m_core.DrawCircle(c, radius, color);
}

void GameState::DrawCircleSolid(Vec2f c, float radius, Color color)
{
	m_core.DrawCircleSolid(c, radius, color);
}

void GameState::DrawArrowWS(Vec2f origin, Vec2f direction, float scale, Color color)
{
	Vec2f left(-direction.y, direction.x);
	Vec2f right(direction.y, -direction.x);

	Vec2f originSS = viewport.WorldToScreenSpace(origin);
	Vec2f targetSS = viewport.WorldToScreenSpace(origin + direction * scale);

	Vec2f rightChev = viewport.WorldToScreenSpace(origin + direction * scale + right * 0.2f * scale - direction * 0.2f * scale);
	Vec2f leftChev = viewport.WorldToScreenSpace(origin + direction * scale + left * 0.2f * scale - direction * 0.2f * scale);

	DrawLine(originSS, targetSS, color);
	DrawLine(targetSS, rightChev, color);
	DrawLine(targetSS, leftChev, color);
}

void GameState::DrawText(const std::string& str, int x, int y)
{
	m_core.DrawText(str, x, y);
}

int GameState::GetWindowWidth() 
{ 
	return m_core.GetWindowWidth(); 
}

int GameState::GetWindowHeight() 
{ 
	return m_core.GetWindowHeight(); 
}

unsigned int GameState::GetCurrentTime()
{
	return m_core.GetCurrentTime();
}

float GameState::GetDeltaTime()
{
	return m_core.GetDeltaTime();
}

Vec2f GameState::GetMousePosition()
{
	return m_core.GetMousePosition();
}

void GameState::RenderPixelMapToTexture(const Map2D<Color>& map, Texture& texture)
{
	m_core.RenderPixelMapToTexture(map, texture);
}
