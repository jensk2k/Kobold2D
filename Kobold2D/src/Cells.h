#pragma once
#include "GameState.h"
#include "TrailMap.h"
#include "Timer.h"
#include "MathUtils.h"
#include "Histogram.h"

struct Genes
{
	int generation = 1;
	Color color = Colors::RED;
	float matureSize = 0.f;
	float startSize = 0.f;
	float maxSpeed = 0.f;
	float detectionRange = 0.f;
	float propagationInterval = 0.f;
	float propagationMinEnergy = 0.f;
	float mutationProbability = 0.f;// percentage
	float lifespan = 0.f;
	float matureAge = 0.f;
	float diet = 0.f; // 0 = herbivore, 1 = carnivore
};

struct NearestFood
{
	Vec2f position;
	bool exists = false;
};

struct Cell
{
	Vec2f position;
	Vec2f velocity;
	Vec2f steeringDirection;
	NearestFood nearestPlant;
	NearestFood nearestMeat;
	Genes genes;
	float energy;
	Timer propagationTimer;
	float age = 0.f;

	float Size() const 
	{ 
		return genes.startSize + (MathUtils::Min(1.f, (age / genes.matureAge)) * (genes.matureSize - genes.startSize)); 
	}
	float MaxEnergy() const { return Size() * 10.f; }
};

enum class FoodState
{
	Growing,
	Plant,
	Meat,
};

struct Food
{
	Timer growthTimer = 0.f;
	FoodState state = FoodState::Growing;
};

class Cells : public GameState
{
public:
	Cells(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;

private:
	void Reset();
	void PropagateCell(Vec2f spawnPosition, const Genes* parentGenes, bool disableMutation);
	Vec2f RandomScreenPosition();
	void CalcSteering(Vec2f steeringDirection, bool shouldBreak, float maxSpeed, float acceleration, float deceleration, Vec2f& position, Vec2f& velocity);
	int GetLastestGen() const;
	float MutateGene(float parentGene, const std::string& name, float min, float max = -1.f)  const;
	Vec2i WorldToFoodCoord(Vec2f worldPos) const;

	Viewport viewport;
	TrailMap trailMap;
	std::vector<Cell> cells;
	Map2D<Food> food;

	float histogramSampleTimer = 0.f;

	Timer metabolismTimer;

	float simTime = 0.f;

	std::map<std::string, Histogram> histograms;
};

