#include "NeuralNetworkDrawer.h"
#include "GameState.h"
#include "NeuralNetwork.h"
#include "Matrix.h"
#include <sstream>

NeuralNetworkDrawer::NeuralNetworkDrawer(GameState& gameState, const NeuralNetwork& nn)
	: gameState(gameState)
	, nn(nn)
{}

void NeuralNetworkDrawer::DrawNeuralNetwork(const Matrix* input, Vec2i topLeft, int width, int height, bool drawText, int neuronRadius)
{
	gameState.DrawRectangle(topLeft, width, height, Colors::WHITE);

	// Draw weights
	for (int layer = 0; layer < nn.weights.size(); layer++)
	{
		for (int col = 0; col < nn.weights[layer].Columns(); col++)
		{
			for (int row = 0; row < nn.weights[layer].Rows(); row++)
			{
				float weight = nn.weights[layer](col, row);

				Vec2i neuronPos1 = GetNeuronDrawPosition(topLeft, width, height, nn, layer, col);
				Vec2i neuronPos2 = GetNeuronDrawPosition(topLeft, width, height, nn, layer + 1, row);

				Vec2f dir = (Vec2f((float)neuronPos2.x, (float)neuronPos2.y) - Vec2f((float)neuronPos1.x, (float)neuronPos1.y)).Normalized();
				Vec2i offset((int)dir.x * neuronRadius - 1, (int)dir.y * neuronRadius - 1);

				float sigW = Sigmoid(weight); // to flatten

											  //Color c = weight < 0.f ? Color(sigW * 255, 0, 0) : Color(0, 0, sigW * 255);
											  //Color c(255 * sigW, 255 * sigW, 255 * sigW);
				Color c = Color::Lerp(Colors::RED, Colors::BLUE, sigW);

				gameState.DrawLine(neuronPos1 + offset, neuronPos2 - offset, c);

				if (drawText)
				{
					std::ostringstream ss;
					ss << (int)(10 * weight) / 10.f;

					Vec2i textPos = neuronPos1 + ((neuronPos2 - neuronPos1) / 4);

					gameState.DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
				}
			}
		}
	}

	std::vector<Matrix> activations;
	//activations.resize(nn.layerShapes.size());
	if (input)
		nn.CalcActivations(*input, activations);

	// Draw nodes
	for (int i = 0; i < nn.layerShapes.size(); i++)
	{
		// neuron
		for (unsigned j = 0; j < nn.layerShapes[i]; j++)
		{
			float neuronActivaion = input ? activations[i](0, j) : 0.f;

			Vec2i centerPos = GetNeuronDrawPosition(topLeft, width, height, nn, i, j);

			if (input)
				gameState.DrawCircleSolid(centerPos, neuronRadius, Color(255 * neuronActivaion, 255 * neuronActivaion, 255 * neuronActivaion));
			gameState.DrawCircle(centerPos, neuronRadius, Colors::WHITE);

			if (drawText && i > 0)
			{
				float bias = nn.biases[i - 1](0, j);

				std::ostringstream ss;
				ss << (int)(10 * bias) / 10.f;

				gameState.DrawText(ss.str(), centerPos.x - 16, centerPos.y - 13 - 32);
			}
		}
	}
}

Vec2i NeuralNetworkDrawer::GetNeuronDrawPosition(Vec2i topLeft, int rectWidth, int rectHeight, const NeuralNetwork& nn, int layerIndex, int neuronIndex)
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
