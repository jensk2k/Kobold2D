#pragma once
#include "GameState.h"
#include <vector>

#define POPULATION 225
#define MUTATION_RATE 0.25

class GeneticAlgorithmDemo : public GameState
{
public:
	GeneticAlgorithmDemo(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
	void CalcFitness();
	void Initialize();
	void Iterate();
private:
	std::vector<std::string> population;
	std::vector<float> fitnesses;
	std::string target;
	std::string best;
	bool isRunning = false;
	int generation = 0;

	float highestFitness = 0.f;
};

