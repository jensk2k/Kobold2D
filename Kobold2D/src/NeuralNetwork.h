#pragma once
#include "Matrix.h"
#include <vector>

struct NeuralNetwork
{
	NeuralNetwork(float learningRate, const std::vector<unsigned>& layerShapes);
	void RandomizeWeightsAndBiases();
	Matrix Solve(const Matrix& input) const;
	void CalcActivations(const Matrix& inputs, std::vector<Matrix>& activations) const;
	void BackPropagate(const Matrix& inputs, const Matrix& targets);

	std::vector<Matrix> weights;
	std::vector<Matrix> biases;

	const std::vector<unsigned> layerShapes;

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
