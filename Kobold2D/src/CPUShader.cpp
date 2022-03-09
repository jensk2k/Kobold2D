#include "CPUShader.h"
#include "MathUtils.h"
#include "PrecisionStopWatch.h"
#include "Random.h"
#include <chrono>
#include <sstream>

CPUShader::CPUShader(Core& core)
	: GameState(core)
	, colorMap(RESX, RESY)
	, map(RESX, RESY)
{
	for (int i = 0; i < NBLOBS; i++)
	{
		blobs.push_back(
			{ 
				Vec2f(Random::Range(0, map.m_width), Random::Range(0, map.m_height)),
				Vec2f(Random::Range(-1.f, 1.f), Random::Range(-1.f, 1.f)).Normalized() * 128 / DIVIDER,
				20/DIVIDER
			});
	}

	for (int i = 0; i < randoms.size(); i++)
	{
		randoms[i] = Random::Range(0.f, 1.f);
	}
}

void CPUShader::Update()
{
	PrecisionStopWatch stopWatch;
	for (int i = 0; i < blobs.size(); i++)
	{
		Blob& blob = blobs[i];

		blob.pos += blob.vel * GetDeltaTime();
		
		if (blob.pos.x < 0.f || blob.pos.x > map.m_width)
			blob.vel.x = -blob.vel.x;

		if (blob.pos.y < 0.f || blob.pos.y > map.m_height)
			blob.vel.y = -blob.vel.y;

		blob.pos.x = MathUtils::Clamp(blob.pos.x, 0.f, map.m_width);
		blob.pos.y = MathUtils::Clamp(blob.pos.y, 0.f, map.m_height);
	}

	for (int y = 0; y < map.m_height; y++)
	{
		for (int x = 0; x < map.m_width; x++)
		{
			float sum = 0.f;

			for (int i = 0; i < blobs.size(); i++)
			{
				const Blob& blob = blobs[i];
				Vec2f pos(blob.pos.x, blob.pos.y);
				float radius = blob.radius;

				//Vec2f pos(map.m_width / 3, map.m_height / 2);
				//float radius = 5;

				Vec2f pixelCoord(x, y);
				float distance = (pixelCoord - pos).Length();

				sum += radius / (distance);

				
			}

			map(x, y) = sum;
		}
	}

	float time = GetCurrentTime() / 1000.f;

	Color col1(
		(Uint8)((sin(time * randoms[0]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[1]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[2]) + 1.0f) / 2.0f * 255)
	);

	Color col2(
		(Uint8)((sin(time * randoms[3]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[4]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[5]) + 1.0f) / 2.0f * 255)
	);

	Color col3(
		(Uint8)((sin(time * randoms[6]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[7]) + 1.0f) / 2.0f * 255),
		(Uint8)((sin(time * randoms[8]) + 1.0f) / 2.0f * 255)
	);

	for (int y = 0; y < map.m_height; y++)
	{
		for (int x = 0; x < map.m_width; x++)
		{
			Vec2i pos(map.m_width / 2, map.m_height / 2);
			//float f = MathUtils::Clamp(map(x, y), 0.f, 1.f);
			//float f = MathUtils::Clamp(1.f - map(x, y), 0.f, 1.f);
			float f = MathUtils::Clamp(MathUtils::Abs(1.f - map(x, y) * map(x, y)), 0.f, 1.f);

			//Color c = Colors::BLACK;
			//if (f > 0.8f && f < 1.f)
			//	c = Color(f * 255, f * 255, f * 255);
			//colorMap(x, y) = c;
			//colorMap(x, y) = Color(f * 255, (1.f-f)  * 255, (1.f-f) * 255);
			//colorMap(x, y) = Color(f * 255, f * 255, f * 255);

			Color c = f < 0.5f ? Color::Lerp(col1, col2, f * 2.f) : Color::Lerp(col2, col3, (f - 0.5f) * 2.0f);

			colorMap(x, y) = c;
			//colorMap(x, y) = Color(f * col1.red, f * col1.green, f * col1.blue);
			//colorMap(x, y) = Color(f * col1.red, (1.f - f) * col1.green, (1.f - f) * col1.blue);
		}
	} 

	updateTime = stopWatch.Stop();
}

void CPUShader::Render()
{
	PrecisionStopWatch stopWatch;

	RenderPixelMapToTexture(colorMap, texture);
	DrawTexture(texture, Vec2i(), GetWindowWidth(), GetWindowHeight());
	for (int i = 0; i < blobs.size(); i++)
	{
		const Blob& blob = blobs[i];
		//DrawCircle(Vec2i(blob.pos.x, blob.pos.y) * 8, 12 * 8, Colors::RED);
	}
	
	renderTime = stopWatch.Stop();

	if (IsFPSCounterEnabled())
	{
		std::ostringstream ss;
		ss << colorMap.m_width << "זרו x" << colorMap.m_height << "(" << colorMap.m_height * colorMap.m_width << ")\r" << renderTime << "\r" << updateTime;
		Vec2i textPos = Vec2i(0, 30);

		DrawText(ss.str(), textPos.x, textPos.y);
	}
}
