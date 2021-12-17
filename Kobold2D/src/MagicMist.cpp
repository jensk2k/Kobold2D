#include "MagicMist.h"

#include "Core.h"
#include "Instrumentor.h"


static void GenerateFractalNoiseMap(Map2D<float>& noiseMap, const PerlinNoise& perlin, float xOffset, float yOffset)
{
	PROFILE_FUNCTION();
	int n = 5;

	float amplitudeMult = 0.5f;
	float frequency = 0.004f * PIXEL_SCALE;
	float frequencyMult = 1.8f;


	for (int y = 0; y < noiseMap.m_height; ++y)
	{
		for (int x = 0; x < noiseMap.m_width; ++x)
		{
			float noise = 0.f;

			float sampleX = x * frequency;
			float sampleY = (y + yOffset) * frequency;
			float amplitude = 1.f;

			for (int i = 0; i < n; ++i)
			{
				float noiseN = (float)perlin.noise(sampleX, sampleY, 0.f);
				noise += noiseN * amplitude;

				sampleX *= frequencyMult;
				sampleY *= frequencyMult;


				amplitude *= amplitudeMult;
			}
			noise /= n;

			noiseMap(x, y) = noise;
		}
	}
}

static float Lerp(float v1, float v2, float l)
{
	return v1 + l * (v2 - v1);
}

static Color LerpColor(Color c1, Color c2, float l)
{
	Uint8 r = (Uint8)(Lerp(c1.red, c2.red, l));
	Uint8 g = (Uint8)(Lerp(c1.green, c2.green, l));
	Uint8 b = (Uint8)(Lerp(c1.blue, c2.blue, l));
	Uint8 a = (Uint8)(Lerp(c1.alpha, c2.alpha, l));

	return Color{ r, g, b, a };
}

static void NoiseToColorMap(const Map2D<float>& noiseMap, Map2D<Color>& colorMapOut, unsigned int currentTime)
{
	PROFILE_FUNCTION();
	int w = noiseMap.m_width;
	int h = noiseMap.m_height;

#if 1
	Uint8 r1 = (Uint8)((cos(currentTime * 0.00021f) + 1.0f) / 2.0f * 255);
	Uint8 g1 = (Uint8)((cos(currentTime * 0.00041f) + 1.0f) / 2.0f * 255);
	Uint8 b1 = (Uint8)((cos(currentTime * 0.00061f) + 1.0f) / 2.0f * 255);

	Uint8 r2 = (Uint8)((sin(currentTime * 0.00031f) + 1.0f) / 2.0f * 255);
	Uint8 g2 = (Uint8)((sin(currentTime * 0.00021f) + 1.0f) / 2.0f * 255);
	Uint8 b2 = (Uint8)((sin(currentTime * 0.00011f) + 1.0f) / 2.0f * 255);

	Uint8 r3 = (Uint8)((sin(currentTime * 0.00041f) + 1.0f) / 2.0f * 255);
	Uint8 g3 = (Uint8)((sin(currentTime * 0.00051f) + 1.0f) / 2.0f * 255);
	Uint8 b3 = (Uint8)((sin(currentTime * 0.00061f) + 1.0f) / 2.0f * 255);
#else
	Uint8 r1 = 64;
	Uint8 g1 = 224;
	Uint8 b1 = 208;

	Uint8 r2 = 255;
	Uint8 g2 = 255;
	Uint8 b2 = 255;

	//Uint8 r3 = 0;
	//Uint8 g3 = 0;
	//Uint8 b3 = 0;
#endif

	Color c1 = { r1, g1, b1 };
	Color c2 = { r2, g2, b2 };
	Color c3 = { r3, g3, b3 };


	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			float noise = noiseMap(x, y);

			Color c = noise < 0.5f ? LerpColor(c1, c2, noise * 2.f) : LerpColor(c2, c3, (noise - 0.5f) * 2.0f);

			colorMapOut(x, y) = c;
		}
	}
}

static void NormalizePixelMap(Map2D<float>& map)
{
	PROFILE_FUNCTION();
	static float maxNoise = 0.0f;
	static float minNoise = 1.0f;

	for (int y = 0; y < map.m_height; ++y)
	{
		for (int x = 0; x < map.m_width; ++x)
		{
			float noise = map(x, y);
			if (noise > maxNoise)
				maxNoise = noise;

			if (noise < minNoise)
				minNoise = noise;
		}
	}

	for (int y = 0; y < map.m_height; ++y)
	{
		for (int x = 0; x < map.m_width; ++x)
		{
			map(x, y) = (map(x, y) - minNoise) / (maxNoise - minNoise);
		}
	}
}


void ProcessNoiseWorker(Map2D<float>* tempMap, const PerlinNoise* perlin, float* yOffset/*, Uint32* totalTimeOut*/)
{
	//Uint32 startTime = SDL_GetTicks();
	GenerateFractalNoiseMap(*tempMap, *perlin, 0.f, *yOffset);
	//*totalTimeOut = SDL_GetTicks() - startTime;
}

MagicMist::MagicMist(Core& core) 
	: GameState(core)
	, m_noise1(NOISE_W, NOISE_H)
	, m_noise2(NOISE_W, NOISE_H)
	, m_noiseMixed(NOISE_W, NOISE_H)
#if MULTI_THREADED
	, m_noise1_temp(NOISE_W, NOISE_H)
	, m_noise2_temp(NOISE_W, NOISE_H)
#endif
	, m_perlin1(rand())
	, m_perlin2(rand())
{
#if MULTI_THREADED
	float iniOffset = 0.f;
	m_t1 = std::thread(ProcessNoiseWorker, &m_noise1_temp, &m_perlin1, &iniOffset/*, &m_t1Time*/);
	m_t2 = std::thread(ProcessNoiseWorker, &m_noise2_temp, &m_perlin2, &iniOffset/*, &m_t2Time*/);
#endif
}

MagicMist::~MagicMist()
{
#if MULTI_THREADED
	m_t1.join();
	m_t2.join();
#endif
}

void MagicMist::Render()
{
	unsigned int currentTime = GetCurrentTime();
	// Generate noise

	float yOffset1 = currentTime * 0.03f;
	float yOffset2 = -yOffset1;

#if MULTI_THREADED
	m_t1.join();
	m_noise1 = m_noise1_temp;
	m_t1 = std::thread(ProcessNoiseWorker, &m_noise1_temp, &m_perlin1, &yOffset1/*, &m_t1Time*/);

	m_t2.join();
	m_noise2 = m_noise2_temp;
	m_t2 = std::thread(ProcessNoiseWorker, &m_noise2_temp, &m_perlin2, &yOffset2/*, &m_t2Time*/);
#else
	GenerateFractalNoiseMap(m_noise1, m_perlin1, 0.f, yOffset1);
	GenerateFractalNoiseMap(m_noise2, m_perlin2, 0.f, yOffset2);
#endif

	// Mix
	for (int y = 0; y < m_noiseMixed.m_height; ++y)
	{
		for (int x = 0; x < m_noiseMixed.m_width; ++x)
		{
			m_noiseMixed(x, y) = (m_noise1(x, y) + m_noise2(x, y)) / 2.f;
		}
	}

	NormalizePixelMap(m_noiseMixed);

	//if (m_noiseTexture.sdlTexture)
	//	SDL_DestroyTexture(m_noiseTexture.sdlTexture);

	Map2D<Color> colorMap(NOISE_W, NOISE_H);
	NoiseToColorMap(m_noiseMixed, colorMap, currentTime);

	Texture noiseTexture;
	RenderPixelMapToTexture(colorMap, noiseTexture);
	//SDL_Texture* sdlNoiseTex = RenderPixelMapToTexture(colorMap);
	//Texture noiseTexture(*sdlNoiseTex);	
	//noiseTexture.m_width = NOISE_W;
	//noiseTexture.m_height = NOISE_H;

	DrawTexture(noiseTexture, Vec2i(), SCREEN_W, SCREEN_H);
}
