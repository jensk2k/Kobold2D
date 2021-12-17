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


#define PROFILING 0
#define FULLSCREEN 0
#define SCREEN_W 1280
#define SCREEN_H 720


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
	int GetWindowWidth() { return SCREEN_W; }
	int GetWindowHeight() { return SCREEN_H; }

	void DrawRectangle(Vec2f position, int width, int height, Color color) const;
	void DrawRectangleSolid(Vec2f position, int width, int height, Color color) const;
	void DrawTexture(const Texture& texture, Vec2f position, int width, int height);
	void DrawLine(Vec2f v1, Vec2f v2, Color color);
	void DrawCircle(Vec2f c, float radius, Color color);
	void DrawCircleSolid(Vec2f c, float radius, Color color);
	void DrawText(const std::string& str, int x, int y);

	SDL_Texture* LoadSDLTexture(const char* texturePath);
	void LoadTexture(const char* texturePath, Texture& texture, int width, int height);
	SDL_Texture* TextToTexture(TTF_Font* font, const char* text);
	//SDL_Texture* RenderPixelMapToTexture(const Map2D<Color>& map);
	void RenderPixelMapToTexture(const Map2D<Color>& map, Texture& textureOUT);

	bool IsRunning() { return m_isRunning; };
	unsigned int GetCurrentTime() { return m_currentTime; }
	Vec2f GetMousePosition() { return mousePosition; }
	float GetDeltaTime() { return m_deltaTime; }

private:
	void DrawFPS();

	SDL_Renderer* m_renderer = nullptr;
	SDL_Window* m_window = nullptr;

	bool m_isRunning = false;
	float m_deltaTime = 0.f;
	float m_prevDeltaTime = 0.f;
	unsigned int m_currentTime = 0;
	
	//Texture m_testTexture;
	TTF_Font* m_font = nullptr;

	std::unique_ptr<GameState> m_gameState;

	Vec2f mousePosition;

	Input m_input;
};

