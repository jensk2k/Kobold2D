#include "Histogram.h"
#include "GameState.h"
#include <sstream>

void Histogram::RecordSample(float sample)
{
	histogram.push_back(sample);
}

void Histogram::Draw(GameState& gameState, Vec2i position, int histogramSize, const std::string* title, bool writeHighestLatest, float highestOverride) const
{
	gameState.DrawRectangle(position, histogramSize, histogramSize, Colors::WHITE);

	if (title)
	{
		std::ostringstream ss;
		ss << *title;
		Vec2i textPos = position + Vec2i(0, -30);

		gameState.DrawText(ss.str(), textPos.x, textPos.y);
	}

	if (histogram.size() > 1)
	{
		Vec2i bottomLeft(position.x, position.y + histogramSize);
		float highestSampleValue = 0.f;
		if (highestOverride < 0.f)
		{
			for (int i = 0; i < histogram.size(); i++)
			{
				float sample = histogram[i];
				if (sample > highestSampleValue)
					highestSampleValue = sample;
			}
		}
		else
		{
			highestSampleValue = highestOverride;
		}

		for (int i = 0; i < histogram.size() - 1; i++)
		{
			float sample1 = histogram[i];
			float x1 = bottomLeft.x + ((float)i / (float)(histogram.size() - 1)) * histogramSize;
			float y1 = bottomLeft.y - ((float)sample1 / (float)highestSampleValue) * histogramSize;

			float sample2 = histogram[i + 1];
			float x2 = bottomLeft.x + ((float)(i + 1) / (float)(histogram.size() - 1)) * histogramSize;
			float y2 = bottomLeft.y - ((float)sample2 / (float)highestSampleValue) * histogramSize;
			gameState.DrawLine(Vec2i(x1, y1), Vec2i(x2, y2), Colors::YELLOW);
		}

		if (writeHighestLatest)
		{
			std::ostringstream ss;
			ss << ((int)(100.f * highestSampleValue)) / 100.f << " " << ((int)(100.f * histogram[histogram.size() - 1])) / 100.f;
			Vec2i textPos = position + Vec2i(0, histogramSize - 30);

			gameState.DrawText(ss.str(), textPos.x, textPos.y);
		}
	}
}

void Histogram::Reset()
{
	histogram.clear();
}
