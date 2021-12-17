#include "Core.h"
#include <iostream>
#include <string>
#include <sstream>

#include "Instrumentor.h"
#include "MagicMist.h"
#include "ProcIK.h"
#include "CreatureGeneration.h"
#include "RayMarching2D.h"
#include "LifeSim.h"
#include "FractalNoiseViever.h"
#include "Tanks.h"
#include "Squares.h"

Core::Core() 
{	
	//m_gameState = std::make_unique<MagicMist>(*this);
	//m_gameState = std::make_unique<ProcIK>(*this);
	m_gameState = std::make_unique<CreatureGeneration>(*this);
	//m_gameState = std::make_unique<RayMarching2D>(*this);
	//m_gameState = std::make_unique<LifeSim>(*this);
	//m_gameState = std::make_unique<FractalNoiseViever>(*this);
	//m_gameState = std::make_unique<Tanks>(*this);
	//m_gameState = std::make_unique<Squares>(*this);
}

Core::~Core()
{
}

int Core::Init()
{
	INSTRUMENTOR_BEGIN();

	int flags = 0;
	if (FULLSCREEN)
		flags |= SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "SDL subsystems initialized" << std::endl;

		m_window = SDL_CreateWindow("Kobold2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, flags);
		if (!m_window)
		{
			SDL_Log("Could not create a window: %s", SDL_GetError());
			return -1;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if (!m_renderer)
		{
			SDL_Log("Could not create a renderer: %s", SDL_GetError());
			return -1;
		}

		//{
		//	m_testTexture.sdlTexture = LoadTexture("assets/moon-stars.jpg");
		//	m_testTexture.w = 626;
		//	m_testTexture.h = 626;
		//}

		SDL_ShowCursor(SDL_DISABLE);

		if (TTF_Init() < 0) {
			std::cout << "Error TTF init" << std::endl;
		}

		//if (m_font = TTF_OpenFont("assets/RobotoMono-Regular.ttf", 24)) {}
		if (m_font = TTF_OpenFont("assets/CenturyGothic.ttf", 24)) {}
		else
		{
			std::cout << "TTF_OpenFont: " << TTF_GetError() << std::endl;
			return -1;
		}

		m_gameState->Init();

		m_isRunning = true;
	}
	return 0;
}

void Core::Clean()
{
	std::cout << "Quiting..." << std::endl;
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
	INSTRUMENTOR_END();
}

void Core::HandleEvents()
{
	PROFILE_FUNCTION();
	SDL_Event m_event;
	SDL_PollEvent(&m_event);

	switch (m_event.type)
	{
		case SDL_KEYDOWN:
		{
			//std::cout << "Keydown: " << m_event.key.keysym.sym << std::endl;

			if (m_event.key.keysym.sym == SDLK_ESCAPE)
			{
				m_isRunning = false;
				break;
			}

			Keys key = m_input.GetKeyMapping(m_event.key.keysym.sym);

			if (key == Keys::INVALID)
			{
				std::cout << "Unmapped keycode: " << m_event.key.keysym.sym << std::endl;
				break;
			}

			//std::cout << "Keydown: " << (int)key << std::endl;
			m_gameState->HandleKeyDown(key);
			break;
		}
		case SDL_KEYUP:
		{
			Keys key = m_input.GetKeyMapping(m_event.key.keysym.sym);

			if (key == Keys::INVALID)
			{
				break;
			}
			//std::cout << "Keyup: " << (int)key << std::endl;
			m_gameState->HandleKeyUp(key);
			break;
		}
		case SDL_QUIT:
		{
			m_isRunning = false;
			break;
		}
		default:
		{
			//std::cout << "Unmapped event type: " << (Uint32)m_event.type << std::endl;
		}
	}

	SDL_PumpEvents();
	int x, y;
	SDL_GetMouseState(&x, &y);
	mousePosition.x = static_cast<float>(x);
	mousePosition.y = static_cast<float>(y);
}

void Core::Update()
{
	PROFILE_FUNCTION();
	m_gameState->Update();
}

void Core::Render()
{
	PROFILE_FUNCTION();
	static Color clearColor = Colors::CHARCOAL;
	SDL_SetRenderDrawColor(m_renderer, clearColor.red, clearColor.green, clearColor.blue, 255);

	SDL_RenderClear(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

	m_gameState->Render();

	DrawFPS();
	//DrawStats();

	SDL_RenderPresent(m_renderer);

	m_prevDeltaTime = m_deltaTime;
	m_deltaTime = (SDL_GetTicks() - m_currentTime) / 1000.f;

	m_currentTime = SDL_GetTicks();
}

int Core::GetDisplayRefreshRate()
{
	int display_count = 0, display_index = 0, mode_index = 0;
	SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
	if (SDL_GetDisplayMode(display_index, mode_index, &mode) != 0) {
		SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
		return 1;
	}
	return mode.refresh_rate;
}

void Core::DrawRectangle(Vec2f position, int width, int height, Color color) const
{
	PROFILE_FUNCTION();
	SDL_SetRenderDrawColor(m_renderer, color.red, color.green, color.blue, color.alpha);
	SDL_Rect rect;
	rect.x = static_cast<int>(position.x);
	rect.y = static_cast<int>(position.y);
	rect.w = width;
	rect.h = height;
	
	SDL_RenderDrawRect(m_renderer, &rect);
}

void Core::DrawRectangleSolid(Vec2f position, int width, int height, Color color) const
{
	PROFILE_FUNCTION();
	SDL_SetRenderDrawColor(m_renderer, color.red, color.green, color.blue, color.alpha);
	SDL_Rect rect;
	rect.x = static_cast<int>(position.x);
	rect.y = static_cast<int>(position.y);
	rect.w = width;
	rect.h = height;
	SDL_RenderFillRect(m_renderer, &rect);
}

void Core::DrawTexture(const Texture& texture, Vec2f position, int width, int height)
{
	PROFILE_FUNCTION();
	SDL_Rect dRect;
	dRect.x = static_cast<int>(position.x);
	dRect.y = static_cast<int>(position.y);
	dRect.w = width;
	dRect.h = height;
	SDL_RenderCopy(m_renderer, &texture.GetSDLTexture(), nullptr, &dRect);
}

void Core::DrawLine(Vec2f v1, Vec2f v2, Color color)
{
	PROFILE_FUNCTION();
	SDL_SetRenderDrawColor(m_renderer, color.red, color.green, color.blue, color.alpha);
	SDL_RenderDrawLineF(m_renderer, v1.x, v1.y, v2.x, v2.y);
}

void Core::DrawCircle(Vec2f c, float radius, Color color)
{
	PROFILE_FUNCTION();
	SDL_SetRenderDrawColor(m_renderer, color.red, color.green, color.blue, color.alpha);

	int offsetx, offsety, d;
	int status;
	int r = static_cast<int>(radius);

	offsetx = 0;
	offsety = r;
	d = r - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(m_renderer, c.x + offsetx, c.y + offsety);
		status += SDL_RenderDrawPoint(m_renderer, c.x + offsety, c.y + offsetx);
		status += SDL_RenderDrawPoint(m_renderer, c.x - offsetx, c.y + offsety);
		status += SDL_RenderDrawPoint(m_renderer, c.x - offsety, c.y + offsetx);
		status += SDL_RenderDrawPoint(m_renderer, c.x + offsetx, c.y - offsety);
		status += SDL_RenderDrawPoint(m_renderer, c.x + offsety, c.y - offsetx);
		status += SDL_RenderDrawPoint(m_renderer, c.x - offsetx, c.y - offsety);
		status += SDL_RenderDrawPoint(m_renderer, c.x - offsety, c.y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	//return status;
}

void Core::DrawCircleSolid(Vec2f c, float radius, Color color)
{
	PROFILE_FUNCTION();
	SDL_SetRenderDrawColor(m_renderer, color.red, color.green, color.blue, color.alpha);

	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(m_renderer, c.x - offsety, c.y + offsetx, c.x + offsety, c.y + offsetx);
		status += SDL_RenderDrawLine(m_renderer, c.x - offsetx, c.y + offsety, c.x + offsetx, c.y + offsety);
		status += SDL_RenderDrawLine(m_renderer, c.x - offsetx, c.y - offsety, c.x + offsetx, c.y - offsety);
		status += SDL_RenderDrawLine(m_renderer, c.x - offsety, c.y - offsetx, c.x + offsety, c.y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}

void Core::DrawText(const std::string& str, int x, int y)
{
	PROFILE_FUNCTION();
	SDL_Texture* texture;

	texture = TextToTexture(m_font, str.c_str());

	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	// Draw
	SDL_Rect textRect;
	textRect.x = x;
	textRect.y = y;
	textRect.w = w;
	textRect.h = h;

	SDL_RenderCopy(m_renderer, texture, nullptr, &textRect);

	SDL_DestroyTexture(texture);
}

SDL_Texture* Core::LoadSDLTexture(const char* texturePath)
{
	PROFILE_FUNCTION();
	SDL_Texture* texture = nullptr;

	SDL_Surface* surface = IMG_Load(texturePath);
	if (surface)
	{
		std::cout << texturePath << " loaded!" << std::endl;
		texture = SDL_CreateTextureFromSurface(m_renderer, surface);

		if (!texture)
		{
			printf("LoadSDLTexture failed: %s\n", SDL_GetError());
		}
	}

	SDL_FreeSurface(surface);

	return texture;
}

void Core::LoadTexture(const char* texturePath, Texture& texture, int width, int height)
{
	SDL_Texture* sdlTexture = LoadSDLTexture(texturePath);

	texture.WriteSDLTexture(*sdlTexture);
	texture.m_width = width;
	texture.m_height = height;
}

SDL_Texture* Core::TextToTexture(TTF_Font* font, const char* text)
{
	PROFILE_FUNCTION();
	SDL_Color white = { 255, 255, 255 };

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(font, text, white, SCREEN_W);
	//SDL_Color black = { 255, 0, 0, 0 };
	//SDL_Surface* surfaceMessage = TTF_RenderText_Shaded(font, text, white, black);
	//SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, white);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);

	SDL_FreeSurface(surfaceMessage);

	return tex;
}

void Core::RenderPixelMapToTexture(const Map2D<Color>& map, Texture& textureOUT)
{
	int w = map.m_width;
	int h = map.m_height;

	SDL_Texture* tex = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

	SDL_SetRenderTarget(m_renderer, tex);
	SDL_RenderClear(m_renderer);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			Color c = map(x, y);

			Uint8 r = c.red;
			Uint8 g = c.green;
			Uint8 b = c.blue;

			SDL_SetRenderDrawColor(m_renderer, r, g, b, 255);
			SDL_RenderDrawPoint(m_renderer, x, y);
		}
	}

	SDL_SetRenderTarget(m_renderer, nullptr);

	textureOUT.WriteSDLTexture(*tex);
	textureOUT.m_width = w;
	textureOUT.m_height = h;
}

void Core::DrawFPS()
{
	PROFILE_FUNCTION();
	std::ostringstream ss;
	float deltaTime = static_cast<float>(m_deltaTime);
	float prevDeltaTime = static_cast<float>(m_prevDeltaTime);
	ss << floor(1.f / ((deltaTime + prevDeltaTime) / 2.f));

	DrawText(ss.str(), 0, 0);
}
