#pragma once
#include "GameState.h"
#include <vector>
#include <array>


#define DIVIDER 4
#define RESX 1280/DIVIDER
#define RESY 720/DIVIDER
#define NBLOBS 24

struct Blob
{
	Vec2f pos;
	Vec2f vel;
	float radius; 
	Color color;
};

class CPUShader : public GameState
{
public:
	CPUShader(Core& core);
	void Update() override;
	void Render() override;

private:
	Map2D<Color> colorMap;
	Map2D<float> map;
	Texture texture;
	int renderTime = 0;
	int updateTime = 0;

	std::vector<Blob> blobs;

	std::array<float, 9> randoms;
};

