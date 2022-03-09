#pragma once
#include "Matrix.h"
#include "Vector2.h"
#include <vector>

struct NeuralNetwork
{
	NeuralNetwork(float learningRate, const std::vector<unsigned>& layerShapes);
	void RandomizeWeightsAndBiases();
	Matrix Solve(const Matrix& input) const;
	void CalcActivations(const Matrix& inputs, std::vector<Matrix>& activations) const;
	void BackPropagate(const Matrix& inputs, const Matrix& targets);
	void SetLayerShapes(const std::vector<unsigned>& layerShapes);

	std::vector<Matrix> weights;
	std::vector<Matrix> biases;

	std::vector<unsigned> layerShapes;

	const float learningRate;
};

static float Sigmoid(float x)
{
	static float e = 2.71828f;

	return 1.f / (1.f + pow(e, -x));
}

static float PsuedoDerivativeSigmoid(float sigmoidedX)
{
	return sigmoidedX * (1.f - sigmoidedX);
}

static Vec2i GetNeuronDrawPosition(Vec2i topLeft, int rectWidth, int rectHeight, const NeuralNetwork& nn, int layerIndex, int neuronIndex)
{
	int edgeMargin = 40;

	int xDrawSpace = rectWidth - 2 * edgeMargin;
	int yDrawSpace = rectHeight - 2 * edgeMargin;

	int layerCount = nn.layerShapes.size();
	int neuronCountInLayer = nn.layerShapes[layerIndex];

	int x = topLeft.x + edgeMargin + (layerIndex + 0.5f) * (xDrawSpace / layerCount);
	int y = topLeft.y + edgeMargin + (neuronIndex + 0.5f) * (yDrawSpace / neuronCountInLayer);

	return Vec2i(x, y);
}