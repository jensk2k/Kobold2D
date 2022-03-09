#pragma once
#include "GameState.h"
#include "NeuralNetwork.h"
#include "NeuralNetworkDrawer.h"
#include "Histogram.h"

class NNFilter : public GameState
{
public:
	
	NNFilter(Core& core);

	void Init() override;
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
private:
	void RenderNNToTexture(Texture& texture, unsigned width, unsigned height);
	Matrix GetTarget(Matrix input) const;
	float RunQuickTests() const;
	float CalculateFitness() const;
	void Reset();
	void IncrementNNLayerShapes();

	Texture inputTexture;
	Texture outputTexture;
	Texture bestTexture;
	Map2D<Color> inputMap;
	//Map2D<Color> outputMap;
	//Map2D<Color> bestMap;
	NeuralNetwork nn;
	NeuralNetworkDrawer nnDrawer;
	Histogram histogram;

	bool runBackProp = false;
	int backPropCount = 0;
	float lastTestScore = 0.f;
	float bestScore = 0.f;
	std::vector<unsigned> bestLayerShapes;

	float testTimer = 0.f;

	int hiddenLayerIndex = 0;

	int tryCount = 0;
	const int maxTries = 1000;
	const int maxBackProps = 100000;
	std::vector<std::string> paths;
	int pathIndex = 0;

	bool rtModeEnabled = false;
	bool autoIncrementEnabled = false;
};

