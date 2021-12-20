#include "Conway.h"
#include "Random.h"

#define WRAPPED 1

#define FREQUENCY 30.f

#define RESX 160
#define RESY 90

//#define RESX 320
//#define RESY 180

//#define RESX 640
//#define RESY 360

static void Randomize(BMap& map)
{
	for (int y = 0; y < map.m_height; y++)
	{
		for (int x = 0; x < map.m_width; x++)
		{
			map.Set(x, y, Random::Range(0, 32) == 0);
		}
	}
}

#if WRAPPED
static bool CheckCell(const BMap& map, int x, int y)
{
	if (x < 0)
		x = map.m_width - 1;
	if (x >= map.m_width)
		x = 0;
	if (y < 0)
		y = map.m_height - 1;
	if (y >= map.m_height)
		y = 0;

	return map.Get(x, y);
}
#else
static bool CheckCell(const BMap& map, int x, int y)
{
	if (x < 0 || y < 0 || x >= map.m_width || y >= map.m_height)
		return false;
	return map.Get(x, y);
}
#endif

static int GetNumberOfLiveNeighbors(const BMap& map, int x, int y)
{
	int nLiveNeighbors = 0;

	if (CheckCell(map, x - 1, y - 1))
		nLiveNeighbors++;
	if (CheckCell(map, x, y - 1))
		nLiveNeighbors++;
	if (CheckCell(map, x + 1, y - 1))
		nLiveNeighbors++;
	if (CheckCell(map, x - 1, y))
		nLiveNeighbors++;
	if (CheckCell(map, x + 1, y))
		nLiveNeighbors++;
	if (CheckCell(map, x - 1, y + 1))
		nLiveNeighbors++;
	if (CheckCell(map, x, y + 1))
		nLiveNeighbors++;
	if (CheckCell(map, x + 1, y + 1))
		nLiveNeighbors++;

	return nLiveNeighbors;
}

static void CopyMap(const BMap& from, BMap& to)
{
	for (int y = 0; y < from.m_height; y++)
	{
		for (int x = 0; x < from.m_width; x++)
		{
			to.Set(x, y, from.Get(x, y));
		}
	}
}

static bool Detect2FrameStalemate(const BMap& test, const BMap& prev)
{
	for (int y = 0; y < test.m_height; y++)
	{
		for (int x = 0; x < test.m_width; x++)
		{
			if (test.Get(x,y) != prev.Get(x,y))
				return false;
		}
	}
	return true;
}

Conway::Conway(Core& core) 
	: GameState(core)
	, map1(RESX, RESY)
	, map2(RESX, RESY)
	, mapPrev(RESX, RESY)
{
	Randomize(map2);
}

void Conway::Update()
{

	BMap& activeMap = useMap1 ? map1 : map2;
	BMap& inactiveMap = useMap1 ? map2 : map1;

	if (isPaused)
	{
		int size = GetWindowWidth() / RESX;
		Vec2i mousePos = GetMousePosition() / size;

		if (leftMouseButtonDown)
		{
			inactiveMap.Set(mousePos.x, mousePos.y, true);
			activeMap.Set(mousePos.x, mousePos.y, true);
		}
		if (rightMouseButtonDown)
		{
			inactiveMap.Set(mousePos.x, mousePos.y, false);
			activeMap.Set(mousePos.x, mousePos.y, false);
		}
	}

	timer -= GetDeltaTime();
	if (timer > 0.f)
		return;
	timer = 1.f / FREQUENCY;

	if (!isPaused)
	{
		CopyMap(activeMap, mapPrev);

		for (int y = 0; y < activeMap.m_height; y++)
		{
			for (int x = 0; x < activeMap.m_width; x++)
			{
				bool self = inactiveMap.Get(x, y);
				int nLiveNeighbors = GetNumberOfLiveNeighbors(inactiveMap, x, y);

				bool isAlive = false;

				if (self)
				{
					if (nLiveNeighbors < 2)
						isAlive = false;
					else if (nLiveNeighbors == 2 || nLiveNeighbors == 3)
						isAlive = true;
					else if (nLiveNeighbors > 3)
						isAlive = false;
				}
				else
				{
					if (nLiveNeighbors == 3)
						isAlive = true;
				}

				activeMap.Set(x, y, isAlive);
			}
		}


		if (Detect2FrameStalemate(activeMap, mapPrev))
		{
			std::cout << "stalemate" << std::endl;

			Randomize(map1);
			Randomize(map2);
		}

		useMap1 = !useMap1;
	}

	RenderBMapToTexture(activeMap, texture);
}

void Conway::Render()
{
	DrawTexture(texture, Vec2i(), GetWindowWidth(), GetWindowHeight());

	if (isPaused)
	{
		int size = GetWindowWidth() / RESX;
		Vec2i mousePos = GetMousePosition() - Vec2i(size / 2, size / 2);

		DrawRectangleSolid(mousePos, size, size, Colors::RED);
	}
}

void Conway::HandleKeyDown(Keys key)
{
	if (key == Keys::R)
	{
		Randomize(map1);
		Randomize(map2);
	}

	if (key == Keys::SPACE)
	{
		isPaused = !isPaused;
	}
}

void Conway::HandleMouseDown(MouseButtons button, Vec2i mousePosition)
{
	if (button == MouseButtons::LEFT)
		leftMouseButtonDown = true;
	if (button == MouseButtons::RIGHT)
		rightMouseButtonDown = true;
}

void Conway::HandleMouseUp(MouseButtons button, Vec2i mousePosition)
{
	if (button == MouseButtons::LEFT)
		leftMouseButtonDown = false;
	if (button == MouseButtons::RIGHT)
		rightMouseButtonDown = false;
}
