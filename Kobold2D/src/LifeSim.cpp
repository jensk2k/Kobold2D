#include "LifeSim.h"
#include "Random.h"
#include <sstream>
#include "Instrumentor.h"


static Color RandomColor()
{
    return Color(
        Random::Range(0, 255),
        Random::Range(0, 255),
        Random::Range(0, 255),
        255
    );
}

LifeSim::LifeSim(Core& core)
	: GameState(core)
    , grid(40, 22)
    , noiseMap(40, 22)
{
    generator.GenerateFractalNoiseMap(noiseMap, 5, 0.5f, 0.08f, 2.f);
    generator.NormalizePixelMap(noiseMap);
}

void LifeSim::Update()
{
    PROFILE_FUNCTION();
}

void LifeSim::Render()
{
    PROFILE_FUNCTION();
#if 1
    for (int y = 0; y < grid.m_height; y++)
    {
        for (int x = 0; x < grid.m_width; x++)
        {
            int width = 32;
            Vec2i position(x * width, y * width);

            uint8_t c = static_cast<uint8_t>(noiseMap(x, y) * 255);
            Color color(c, c, c, 255);

            DrawRectangleSolid(position, width, width, color);
            DrawRectangle(position, width, width, Colors::WHITE);
        }
    }
#endif
}

