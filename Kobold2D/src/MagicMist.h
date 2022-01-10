#pragma once
#include <thread>
#include "PerlinNoise.h"
#include "Map2D.h"
#include "Texture.h"
#include "GameState.h"

#define PIXEL_SCALE 2

//#define NOISE_W INITSCREEN_W / PIXEL_SCALE
//#define NOISE_H INITSCREEN_H / PIXEL_SCALE

#define NOISE_W 320
#define NOISE_H 180

#define MULTI_THREADED 1


class MagicMist : public GameState
{
public:
	MagicMist(Core& core);
	~MagicMist();

	void Update() override {};
	void Render() override;

private:
	//Texture m_noiseTexture;
	const PerlinNoise m_perlin1;
	const PerlinNoise m_perlin2;
	Map2D<float> m_noise1;
	Map2D<float> m_noise2;
	Map2D<float> m_noiseMixed;
#if MULTI_THREADED
	Map2D<float> m_noise1_temp;
	Map2D<float> m_noise2_temp;
	std::thread m_t1;
	std::thread m_t2;
	Uint32 m_t1Time;
	Uint32 m_t2Time;
#endif
};

