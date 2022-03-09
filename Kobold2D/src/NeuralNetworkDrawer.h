#pragma once
#include "Vector2.h"

class GameState;
struct NeuralNetwork;
class Matrix;

class NeuralNetworkDrawer
{
public:
	NeuralNetworkDrawer(GameState& gameState, const NeuralNetwork& nn);
	void DrawNeuralNetwork(const Matrix* input, Vec2i topLeft, int width, int height, bool drawText, int neuronRadius);
private:
	Vec2i GetNeuronDrawPosition(Vec2i topLeft, int rectWidth, int rectHeight, const NeuralNetwork& nn, int layerIndex, int neuronIndex);
	GameState& gameState;
	const NeuralNetwork& nn;
};

