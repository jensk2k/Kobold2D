#include "squares.h"

Squares::Squares(Core& core)
	: GameState(core)
{

}

void Squares::Update()
{
}

void Squares::Render()
{
	unsigned int time = GetCurrentTime();

	int nX = 64;
	int nY = 36;

	Color color = Color(192, 192, 192, 255);
	float timeScale = 0.004f;
	float squareSize = 12.f;
	float spacing = 20.f;
	float xTimeoffset = 0.08f;
	float yTimeoffset = 0.1f;

	for (int y = 0; y < nY; y++)
	{
		for (int x = 0; x < nX; x++)
		{
			float scaleX = (sin(time * timeScale + x * xTimeoffset) + 2.f) / 3.f;
			float scaleY = (sin(time * timeScale + y * yTimeoffset) + 2.f) / 3.f;

			Vec2i pos;
			pos.x = static_cast<int>(spacing * 0.5f + x * spacing - squareSize * 0.5f * scaleX);
			pos.y = static_cast<int>(spacing * 0.5f + y * spacing - squareSize * 0.5f * scaleY);

			DrawRectangleSolid(pos, 
				static_cast<int>(scaleX * squareSize), 
				static_cast<int>(scaleY * squareSize), 
				color);
		}
	}
}
