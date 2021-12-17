#pragma once
#include "Vector2.h"
#include "Color.h"
#include "Texture.h"
#include "Map2D.h"
#include "Input.h"
#include "Viewport.h"

class Core;

class GameState
{
public:
	GameState(Core& core) 
		: m_core(core)
		, viewport(*this) 
	{}
	virtual ~GameState() {}
	virtual void Init() {};
	virtual void HandleKeyDown(Keys key) {}
	virtual void HandleKeyUp(Keys key) {}
	virtual void Update() = 0;
	virtual void Render() = 0;

	void LoadTexture(const char* texturePath, Texture& texture, int width, int height);
	void DrawRectangle(Vec2f position, int width, int height, Color color) const;
	void DrawRectangleSolid(Vec2f position, int width, int height, Color color) const;
	void DrawTexture(const Texture& texture, Vec2f position, int width, int height);
	void DrawLine(Vec2f v1, Vec2f v2, Color color);
	void DrawCircle(Vec2f c, float radius, Color color);
	void DrawCircleSolid(Vec2f c, float radius, Color color);
	void DrawArrowWS(Vec2f origin, Vec2f direction, float scale, Color colo);
	void DrawText(const std::string& str, int x, int y);
	int GetWindowWidth();
	int GetWindowHeight();
	unsigned int GetCurrentTime();
	float GetDeltaTime();
	Vec2f GetMousePosition();
	void RenderPixelMapToTexture(const Map2D<Color>& map, Texture& texture);
	Viewport viewport;

private:
	Core& m_core;
};
