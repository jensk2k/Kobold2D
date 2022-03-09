#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <memory>

#include "Map2D.h"
#include "Vector2.h"
#include "Texture.h"
#include "Color.h"
#include "Input.h"
#include "KTypes.h"

#define PROFILING 0
#define FULLSCREEN 0
//#define INITSCREEN_W 1280
//#define INITSCREEN_H 720
#define INITSCREEN_W 1920
#define INITSCREEN_H 1080
//#define SCREEN_W 2560
//#define SCREEN_H 1440

class GameState;

class Core
{
public:
	Core();
	~Core();

	int Init();
	void Clean();
	void HandleEvents();
	void Update();
	void Render();
	int GetDisplayRefreshRate();
	int GetWindowWidth() const { return screenBounds.x; }
	int GetWindowHeight() const { return screenBounds.y; }
	Vec2i GetScreenBounds() const { return screenBounds; }

	void DrawRectangle(Vec2i position, int width, int height, Color color) const;
	void DrawRectangleSolid(Vec2i position, int width, int height, Color color) const;
	void DrawTexture(const Texture& texture, Vec2i position, int width, int height);
	void DrawLine(Vec2f v1, Vec2f v2, Color color);
	void DrawLine(Vec2i v1, Vec2i v2, Color color);
	void DrawCircle(Vec2i c, int radius, Color color);
	void DrawCircleSolid(Vec2i c, int radius, Color color);
	void DrawText(const std::string& str, int x, int y);

	SDL_Texture* LoadSDLTexture(const char* texturePath);
	void LoadTexture(const char* texturePath, Texture& texture, int width, int height);
	void LoadImageMap(const char* texturePath, Map2D<Color>& colorMap) const;
	SDL_Texture* TextToTexture(TTF_Font* font, const char* text);
	//SDL_Texture* RenderPixelMapToTexture(const Map2D<Color>& map);
	void RenderPixelMapToTexture(const Map2D<Color>& map, Texture& textureOUT) const;
	void RenderBMapToTexture(const BMap& map, Texture& textureOUT);

	bool IsRunning() { return m_isRunning; };
	unsigned int GetCurrentTime() { return m_currentTime; }
	Vec2i GetMousePosition() { return mousePosition; }
	float GetDeltaTime() { return m_deltaTime; }
	bool IsFPSCounterEnabled() const { return m_FPSCounterEnabled; }

private:
	void DrawFPS();
	void UpdateScreenBounds();

	SDL_Renderer* m_renderer = nullptr;
	SDL_Window* m_window = nullptr;

	bool m_isRunning = false;
	bool m_isFullscreen = FULLSCREEN;
	bool m_FPSCounterEnabled = false;
	float m_deltaTime = 0.f;
	int fpsCounter = 0;
	float fpsTimer = 0.f;
	int fps = 0;
	unsigned int m_currentTime = 0;
	
	TTF_Font* m_font = nullptr;

	std::unique_ptr<GameState> m_gameState;

	Vec2i screenBounds = Vec2i(INITSCREEN_W, INITSCREEN_H);
	Vec2i mousePosition;

	Input m_input;
};

