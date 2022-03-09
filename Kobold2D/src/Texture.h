#pragma once
#include <SDL.h>


struct Texture
{
	Texture() {};

	Texture(SDL_Texture& sdlTexture, int width, int height)
		: m_sdlTexture(&sdlTexture)
		, m_width(width)
		, m_height(height)
	{ }
	Texture(SDL_Texture& sdlTexture)
		: m_sdlTexture(&sdlTexture)
	{ }

	~Texture()
	{
		//if (m_sdlTexture)
		//SDL_DestroyTexture(m_sdlTexture);
		m_sdlTexture = nullptr;
	}

	SDL_Texture& GetSDLTexture() const
	{
		return *m_sdlTexture;
	}

	void WriteSDLTexture(SDL_Texture& sdlTexture)
	{
		if (m_sdlTexture)
			SDL_DestroyTexture(m_sdlTexture);
		m_sdlTexture = &sdlTexture;
	}

	int m_width = 0;
	int m_height = 0;
private:
	SDL_Texture* m_sdlTexture = nullptr;

};
