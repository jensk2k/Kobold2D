#pragma once
#include "GameState.h"
#include "TrailMap.h"

struct Genes
{
	int generation = 1;
	Color color = Colors::RED;
	float size = 0.5f;
	float maxSpeed = 5.f;
	float foodDetectionRange = 20.f;
	float repulsionRange = 5.f;
	float propagationInterval = 5.f;
	float propagationMinEnergy = 0.5f;
	float satiation = 1.f; // percentage
	float mutationProbability = .25f;// percentage

	float MaxEnergy() const { return size * 10.f; }
};

struct Cell
{
	Vec2f position;
	Vec2f velocity;
	Genes genes;
	float energy;
	float propagationTimer = 0.f;

	Vec2f repDirection;
};

class Histogram
{
public:
	void RecordSample(float sample);
	void Draw(GameState& gameState, Vec2i position, int histogramSize, const std::string& title) const;
private:
	std::vector<float> histogram;
};

class Cells : public GameState
{
public:
	Cells(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;

private:
	void PropagateFood();
	void SpeadFood(int n);
	void Reset();
	void PropagateCell(const Cell* parent);
	Vec2f RandomScreenPosition();
	void CalcSteering(Vec2f steeringDirection, bool shouldBreak, float maxSpeed, float acceleration, float deceleration, Vec2f& position, Vec2f& velocity);
	int GetLastestGen() const;
	TrailMap trailMap;
	//void DrawHistogram();
	std::vector<Cell> cells;
	std::vector<Vec2f> food;

	//std::vector<Genes> histogram;
	float histogramSampleTimer = 0.f;

	float tickTimer = 0.f;
	float simTime = 0.f;

	std::map<std::string, Histogram> histograms;

	//Histogram populationHistogram;
	//Histogram sizeHistogram;
	//Histogram speedHistogram;
	//Histogram foodRangeHistogram;
	//Histogram repulsionHistogram;
	//Histogram propagationIntervalHistogram;
	//Histogram propagationMinEnergyHistogram;
	//Histogram satiaionHistogram;
};

