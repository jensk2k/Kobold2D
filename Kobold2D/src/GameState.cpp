#include "GameState.h"
#include "Core.h"

void GameState::LoadTexture(const char* texturePath, Texture& texture, int width, int height)
{
	m_core.LoadTexture(texturePath, texture, width, height);
}

void GameState::LoadImageMap(const char* texturePath, Map2D<Color>& colorMap) const
{
	m_core.LoadImageMap(texturePath, colorMap);
}

void GameState::DrawTexture(const Texture& texture, Vec2i position, int width, int height)
{
	m_core.DrawTexture(texture, position, width, height);
}

void GameState::DrawRectangle(Vec2i position, int width, int height, Color color) const
{
	m_core.DrawRectangle(position, width, height, color);
}

void GameState::DrawRectangleSolid(Vec2i position, int width, int height, Color color) const
{
	m_core.DrawRectangleSolid(position, width, height, color);
}

void GameState::DrawCircle(Vec2i c, int radius, Color color)
{
	m_core.DrawCircle(c, radius, color);
}

void GameState::DrawCircleSolid(Vec2i c, int radius, Color color)
{
	m_core.DrawCircleSolid(c, radius, color);
}

void GameState::DrawLine(Vec2f v1, Vec2f v2, Color color)
{
	m_core.DrawLine(v1, v2, color);
}

void GameState::DrawLine(Vec2i v1, Vec2i v2, Color color)
{
	m_core.DrawLine(v1, v2, color);
}

void GameState::DrawText(const std::string& str, int x, int y)
{
	m_core.DrawText(str, x, y);
}

int GameState::GetWindowWidth() const
{ 
	return m_core.GetWindowWidth(); 
}

int GameState::GetWindowHeight() const
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

bool GameState::IsFPSCounterEnabled() const
{
	return m_core.IsFPSCounterEnabled();
}

Vec2i GameState::GetMousePosition()
{
	return m_core.GetMousePosition();
}

void GameState::RenderPixelMapToTexture(const Map2D<Color>& map, Texture& texture) const
{
	m_core.RenderPixelMapToTexture(map, texture);
}
void GameState::RenderBMapToTexture(const BMap& map, Texture& texture)
{
	m_core.RenderBMapToTexture(map, texture);
}