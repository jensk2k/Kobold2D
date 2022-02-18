#pragma once
#include "GameState.h"
#include "Matrix.h"
#include <vector>
#include "Histogram.h"
#include "NeuralNetwork.h"

class NeuralNetworkDemo : public GameState
{
public:
	NeuralNetworkDemo(Core& core);
	void Update() override;
	void Render() override;
	void DrawNeuralNetwork(const NeuralNetwork& nn, Vec2i topLeft, int width, int height);
	Vec2i GetNeuronDrawPosition(Vec2i topLeft, int rectWidth, int rectHeight, const NeuralNetwork& nn, int layerIndex, int neuronIndex);
	void HandleKeyDown(Keys key) override;
	void RunTests();
	void Reset();
private:
	NeuralNetwork nn;
	Matrix displayedInput;

	float lastTestScore = 0.f;
	float bestScore = 0.f;

	Map2D<Color> colorMap;
	Texture tex;
	int backPropCount = 0;

	int backPropsPerSecondCounter = 0;
	float backPropsPerSecondTimer = 0.f;
	int backPropsPerSecond = 0;

	bool runAnimBackProp = false; 
	std::vector<Matrix> trainingData;

	Histogram histogram;
};

