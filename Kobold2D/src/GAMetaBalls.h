#pragma once
#include "GameState.h"
#include "Histogram.h"
#include <vector>

#define POPULATION 100
#define MUTATION_RATE 0.5
#define BLOB_MAX_RADIUS 0.5

struct NNBlob
{
	Vec2f pos;
	float radius;
};

struct Gene
{
	std::vector<Color> colors;
	std::vector<NNBlob> blobs;
};

class GAMetaBalls : public GameState
{
public:
	GAMetaBalls(Core& core);
	void Init() override;
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
private:
	void InitializePopulation();
	void Iterate();
	void RecalculateFitness();
	void Reset();
	void RenderBlobs(const std::vector<NNBlob> blobs, const std::vector<Color>& colors, Map2D<Color>& colorMap, int dimensions) const;
	void LoadInputImage(int index);
	
	Texture inputTex;
	Texture outputTex;
	Texture bestTex;
	Map2D<Color> inputMap;
	Map2D<Color> outputMap;
	Map2D<Color> bestMap;

	std::vector<std::string> paths;
	int inputImageIndex = 0;

	std::vector<Gene> population;
	std::vector<float> fitnesses;

	float averageFitness = 0.f;
	float highestFitness = 0.f;
	float allTimeBest = 0.f;
	int bestSpecimen;

	int generation = 0;
	bool running = false;

	Histogram histogram;
};
