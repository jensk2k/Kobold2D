#include "NeuralNetworkColorDemo.h"
#include <sstream>
#include "Random.h"
#include "MathUtils.h"
#include "PrecisionStopWatch.h"

#define NEURON_RADIUS 20

static float Sigmoid(float x)
{
	static float e = 2.71828f;

	return 1.f / (1.f + pow(e, -x));
}

static float SigmoidDerivative(float x)
{
	float sigX = Sigmoid(x);

	return sigX * (1.f - sigX);
}

static float ReLU(float x)
{
	return MathUtils::Max(0.f, x);
}

NNetwork::NNetwork(std::vector<int>& setup)
{
	// neurons
	layers.resize(setup.size());
	for (int i = 0; i < layers.size(); i++)
	{
		for (int j = 0; j < setup[i]; j++)
		{
			layers[i].push_back(0.f); // activations
			//layers[i].push_back(Random::Range(0.f, 1.f)); // activations
		}
	}

	// biases
	biasesList.resize(setup.size() - 1);
	for (int i = 0; i < biasesList.size(); i++)
	{
		for (int j = 0; j < setup[i + 1]; j++)
		{
			biasesList[i].push_back(Random::Range(-10.f, 10.f)); // bias
		}
	}

	connectionsList.resize(setup.size() - 1);
	for (int i = 0; i < connectionsList.size(); i++)
	{
		int layer1Size = layers[i].size();
		int layer2Size = layers[i + 1].size();

		int nConnetions = layer1Size * layer2Size;

		for (int j = 0; j < nConnetions; j++)
		{
			connectionsList[i].push_back(Random::Range(-4.f, 4.f));
		}
	}
}

NeuralNetworkColorDemo::NeuralNetworkColorDemo(Core& core)
	: GameState(core)
{
	std::vector<int> nnSetup = std::vector<int>{ 3, 4, 6 };

	nn = NNetwork(nnSetup);
}

void NeuralNetworkColorDemo::Update()
{
	if (testing)
	{
		PrecisionStopWatch sw;
		/*static int testsPerFrame = 100;
		for (int i = 0; i < testsPerFrame; i++)*/
		while (sw.GetMilli() < 1.f/30.f)
		{
			RandomizeInput();
			SolveNN();

			if (currentNNAnswer == GetCorrectAnswer(randomInputColor))
				correctGuesses++;

			accumulatedCost += currentCost;

			testIterations++;
			if (testIterations == 5000)
			{
				testing = false;
				break;
			}
			sw.Stop();
		}
	}
}

void NeuralNetworkColorDemo::Render()
{
	DrawAllLayers();
}

void NeuralNetworkColorDemo::HandleKeyDown(Keys key)
{
	if (key == Keys::R)
	{
		RandomizeNN();
	}

	if (key == Keys::E)
	{
		RandomizeInput();
		SolveNN();
	}

	if (key == Keys::T)
	{
		testIterations = 0;
		accumulatedCost = 0.f;
		correctGuesses = 0;
		MassTest();

		//testing = true;
	}

	if (key == Keys::B)
	{
		BackProb();
	}


	if (key == Keys::M)
	{
		std::vector<float> mIniData1= {	1,2,3,
										4,5,6 };

		Matrix m1(3, 2, mIniData1);
		Matrix m2(2, 3, mIniData1);
		Matrix m3(3, 2, mIniData1);

		std::cout << m1 << std::endl;
		std::cout << m2 << std::endl;
		//std::cout << Matrix::Add(m1, m3) << std::endl;
		//std::cout << m1.Add(m3) << std::endl;
		std::cout << Matrix::Product(m1, m2) << std::endl;
	}

}

void NeuralNetworkColorDemo::RandomizeNN()
{
	for (int i = 0; i < nn.connectionsList.size(); i++)
	{
		for (int j = 0; j < nn.connectionsList[i].size(); j++)
		{
			nn.connectionsList[i][j] = Random::Range(-4.f, 4.f);
		}
	}

	for (int i = 0; i < nn.biasesList.size(); i++)
	{
		for (int j = 0; j < nn.biasesList[i].size(); j++)
		{
			nn.biasesList[i][j] = Random::Range(-10.f, 10.f);
		}
	}
}

void NeuralNetworkColorDemo::RandomizeInput()
{
	randomInputColor = Color(Random::Range(0, 255), Random::Range(0, 255), Random::Range(0, 255));

	// set input
	for (int i = 0; i < nn.layers[0].size(); i++)
	{
		nn.layers[0][i] = Random::Range(0.f, 1.f);
	}
}

void NeuralNetworkColorDemo::DrawAllLayers()
{
	Rect2i rect(10, 10, GetWindowWidth() - 20, GetWindowHeight() - 200);
	DrawRectangle(rect.pos, rect.width, rect.height, Colors::WHITE);

	for (int i = 0; i < nn.layers.size(); i++)
	{
		DrawLayer(rect, i);
	}

	for (int i = 0; i < nn.connectionsList.size(); i++)
	{
		DrawConnections(rect, i);
	}

	// labels
	int outputLayer = nn.layers.size() - 1;
	for (int i = 0; i < nn.layers[outputLayer].size(); i++)
	{
		Vec2i neuronPosition = GetNeuronDrawPosition(rect, outputLayer, i);

		std::ostringstream ss;
		std::string label;
		GetOutputLabel(label, i);
		ss << label;

		Vec2i textPos = neuronPosition + Vec2i(60, 0);

		DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
	}

	// Draw input color
	int yStart = rect.height + 20;
	int yEnd = GetWindowHeight() - 10;

	DrawRectangleSolid(Vec2i(10, yStart), 300, yEnd - yStart, randomInputColor);
	DrawRectangle(Vec2i(10, yStart), 300, yEnd - yStart, Colors::WHITE);

	// Draw Answer
	{
		int correct = GetCorrectAnswer(randomInputColor);
		std::string correctLabel;
		GetOutputLabel(correctLabel, correct);

		std::string answerLabel;
		GetOutputLabel(answerLabel, currentNNAnswer);

		float cost = CalcCost(correct);

		std::ostringstream ss;
		ss << "Guess: " << answerLabel
			<< "\nCorrect: " << correctLabel
			<< "\nCost: " << cost
			<< "\nTotal Cost: " << (testIterations > 0 ? (accumulatedCost / testIterations) : 0.f)
			<< "\nTests: " << testIterations
			<< "\nCorrectness: " << 100.f * (testIterations > 0 ? (correctGuesses / (float)testIterations) : 0) << "% " << correctGuesses;

		Vec2i textPos(360, rect.pos.y + rect.height + 40);

		DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
	}
}

void NeuralNetworkColorDemo::DrawLayer(Rect2i rect, int layerIndex)
{
	std::vector<float>& layer = nn.layers[layerIndex];

	for (int i = 0; i < layer.size(); i++)
	{
		float neuronActivaion = layer[i];

		Vec2i centerPos = GetNeuronDrawPosition(rect, layerIndex, i);

		DrawCircleSolid(centerPos, NEURON_RADIUS, Color(255 * neuronActivaion, 255 * neuronActivaion, 255 * neuronActivaion));
		DrawCircle(centerPos, NEURON_RADIUS, Colors::WHITE);

		if (layerIndex == 0)
			continue;

		float bias = GetNeuronBias(layerIndex, i);
		
		std::ostringstream ss;
		ss << (int)(10*bias)/10.f;

		DrawText(ss.str(), centerPos.x -16, centerPos.y-13-32);
	}
}

void NeuralNetworkColorDemo::DrawConnections(Rect2i rect, int connectionsIndex)
{
	int layer1Index = connectionsIndex;
	int layer2Index = connectionsIndex + 1;

	std::vector<float>& layer1 = nn.layers[layer1Index];
	std::vector<float>& layer2 = nn.layers[layer2Index];

	int layer1Size = layer1.size();
	int layer2Size = layer2.size();

	for (int layer1NeuronIndex = 0; layer1NeuronIndex < layer1Size; layer1NeuronIndex++)
	{
		for (int layer2NeuronIndex = 0; layer2NeuronIndex < layer2Size; layer2NeuronIndex++)
		{
			float weight = GetConnectionWeight(layer1Index, layer2Index, layer1NeuronIndex, layer2NeuronIndex);

			Vec2i neuronPos1 = GetNeuronDrawPosition(rect, layer1Index, layer1NeuronIndex);
			Vec2i neuronPos2 = GetNeuronDrawPosition(rect, layer2Index, layer2NeuronIndex);

			Vec2f dir = (Vec2f(neuronPos2.x, neuronPos2.y) - Vec2f(neuronPos1.x, neuronPos1.y)).Normalized();
			Vec2i offset(dir.x * NEURON_RADIUS, dir.y * NEURON_RADIUS);

			float sigW = Sigmoid(MathUtils::Abs(weight));
			//Color c = weight < 0.f 
			//	? Color(255, sigW * 255, sigW * 255)
			//	: Color(sigW * 255, sigW * 255, 255);

			Color c = weight < 0.f 
				? Color(sigW * 255, 0, 0)
				: Color(0, 0, sigW * 255);

			DrawLine(neuronPos1 + offset, neuronPos2 - offset, c);

			std::vector<float> connections = nn.connectionsList[connectionsIndex];


			std::ostringstream ss;
			ss << (int)(10 * weight) / 10.f;

			Vec2i textPos = neuronPos1 + ((neuronPos2 - neuronPos1) / 4);

			DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
		}
	}
}

Vec2i NeuralNetworkColorDemo::GetNeuronDrawPosition(Rect2i rect, int layerIndex, int neuronIndex)
{
	int edgeMargin = 40;

	int xDrawSpace = rect.width - 2 * edgeMargin;
	int yDrawSpace = rect.height - 2 * edgeMargin;

	int x = rect.pos.x + edgeMargin + (layerIndex + 0.5f) * (xDrawSpace / nn.layers.size());
	int y = rect.pos.y + edgeMargin + (neuronIndex + 0.5f) * (yDrawSpace / nn.layers[layerIndex].size());

	return Vec2i(x, y);
}

int NeuralNetworkColorDemo::GetConnectionIndex(int layer1, int layer2, int neuron1, int neuron2)
{
	return nn.layers[layer2].size() * neuron1 + neuron2;
}

float NeuralNetworkColorDemo::GetConnectionWeight(int layer1, int layer2, int neuron1, int neuron2)
{
	int connectionIndex = GetConnectionIndex(layer1, layer2, neuron1, neuron2);

	return nn.connectionsList[layer1][connectionIndex];
}

float NeuralNetworkColorDemo::GetNeuronBias(int layer, int neuron)
{
	return nn.biasesList[layer - 1][neuron];
}

void NeuralNetworkColorDemo::SolveNN()
{
	for (int i = 1; i < nn.layers.size(); i++)
	{
		SolveLayerActivations(i);
	}

	currentNNAnswer = GetNNAnswer();
	int correct = GetCorrectAnswer(randomInputColor);
	currentCost = CalcCost(correct);
}

void NeuralNetworkColorDemo::SolveLayerActivations(int layer)
{
	for (int i = 0; i < nn.layers[layer].size(); i++)
	{
		float neuronActivation = 0.f;

		for (int j = 0; j < nn.layers[layer - 1].size(); j++)
		{
			float weight = GetConnectionWeight(layer - 1, layer, j, i);
			float l1Activaton = Sigmoid(nn.layers[layer - 1][j]);
			neuronActivation += weight * l1Activaton;
		}

		float bias = nn.biasesList[layer-1][i];

		float unsigmoided = neuronActivation + bias;

		nn.layers[layer][i] = unsigmoided;
	}
}

int NeuralNetworkColorDemo::GetNNAnswer()
{
	int answer = 0;
	float bestActivation = 0.f;
	int outputLayer = nn.layers.size() - 1;
	for (int i = 0; i < nn.layers[outputLayer].size(); i++)
	{
		float outputActivation = Sigmoid(nn.layers[outputLayer][i]);
		if (outputActivation > bestActivation)
		{
			bestActivation = outputActivation;
			answer = i;
		}
	}
	return answer;
}

float NeuralNetworkColorDemo::CalcCost(int correct)
{
	float cost = 0.f;

	int outputLayer = nn.layers.size() - 1;
	for (int i = 0; i < nn.layers[outputLayer].size(); i++)
	{
		float outputNeuron = Sigmoid(nn.layers[outputLayer][i]);
		float correctForNeuron = i == correct ? 1.f : 0.f;

		cost += MathUtils::Square(outputNeuron - correctForNeuron);
	}

	return cost;
}

void NeuralNetworkColorDemo::MassTest()
{
	PrecisionStopWatch sw;
	/*static int testsPerFrame = 100;
	for (int i = 0; i < testsPerFrame; i++)*/
	for (int i = 0; i < 5000; i++)
	{
		RandomizeInput();
		SolveNN();

		if (currentNNAnswer == GetCorrectAnswer(randomInputColor))
			correctGuesses++;

		accumulatedCost += currentCost;

		testIterations++;

	}
}

void NeuralNetworkColorDemo::GetOutputLabel(std::string& label, int outputNeuron)
{
	switch (outputNeuron)
	{
	case 0:
		label = "red";
		break;
	case 1:
		label = "green";
		break;
	case 2:
		label = "blue";
		break;
	case 3:
		label = "yellow";
		break;
	case 4:
		label = "cyan";
		break;
	case 5:
		label = "magenta";
		break;
	default:
		label = "undefined";
		break;
	}
}

int NeuralNetworkColorDemo::GetCorrectAnswer(Color color)
{
	int answer = 0;
	int best = 255 + 255 + 255;

	int redDiff = GetColorDiff(Colors::RED, color);
	int greenDiff = GetColorDiff(Colors::GREEN, color);
	int blueDiff = GetColorDiff(Colors::BLUE, color);
	int yellowDiff = GetColorDiff(Colors::YELLOW, color);
	int cyanDiff = GetColorDiff(Colors::CYAN, color);
	int magentaDiff = GetColorDiff(Colors::MAGENTA, color);

	if (redDiff < best)
	{
		best = redDiff;
		answer = 0;
	}
	if (greenDiff < best)
	{
		best = greenDiff;
		answer = 1;
	}
	if (blueDiff < best)
	{
		best = blueDiff;
		answer = 2;
	}
	if (yellowDiff < best)
	{
		best = yellowDiff;
		answer = 3;
	}
	if (cyanDiff < best)
	{
		best = cyanDiff;
		answer = 4;
	}
	if (magentaDiff < best)
	{
		best = magentaDiff;
		answer = 5;
	}

	return answer;
}

int NeuralNetworkColorDemo::GetColorDiff(Color color1, Color color2)
{
	return MathUtils::Abs(color1.red - color2.red)
		+ MathUtils::Abs(color1.green - color2.green)
		+ MathUtils::Abs(color1.blue - color2.blue);
}

void NeuralNetworkColorDemo::BackProb()
{
	// ini accumulated gradient 
	NNGradient backProbNNG;
	backProbNNG.Resize(nn);
	int numberOfTrainingExamples = 100;


	// calc ini accumulated gradient  gradient
	for (int j = 0; j < numberOfTrainingExamples; j++)
	{
		// ini single gradient 
		NNGradient nng;
		nng.Resize(nn);


		Color trainingDataEntry(Random::Range(0, 255), Random::Range(0, 255), Random::Range(0, 255));
		int correct = GetCorrectAnswer(trainingDataEntry);

		randomInputColor = trainingDataEntry;
		SolveNN();

		float cost0 = CalcCost(correct);

		float cost0_der = 0.f;

		for (int k = 0; k < nn.layers[nn.layers.size() - 1].size(); k++)
		{
			float z = nn.layers[nn.layers.size() - 1][k];
			float activation = Sigmoid(z);
			float y_j = k == correct ? 1.f : 0.f;

			cost0_der += 2.f * (activation - y_j);
		}




		// loop backward through layers
		for (int layerL = nn.layers.size() - 1; layerL > 0; layerL--)
		{
			int layerLminus1 = layerL - 1;

			// calc weights gradient per layers
			// connection loop
			for (int neuronJ = 0; neuronJ < nn.layers[layerL].size(); neuronJ++)
			{
				float zL = nn.layers[layerL][neuronJ];
				float aL = Sigmoid(zL);
				float aL_zL_der = SigmoidDerivative(zL); // sig_der(zL)
				float c0_aL_der = cost0_der; // 2(aL - y)

				for (int neuronK = 0; neuronK < nn.layers[layerLminus1].size(); neuronK++)
				{
					int connectionIndex = GetConnectionIndex(layerLminus1, layerL, neuronK, neuronJ);

					float aLminus1 = Sigmoid(nn.layers[layerLminus1][neuronK]);

					float zL_wL_der = aLminus1; // aL-1

					float c0_wL_der = zL_wL_der * aL_zL_der * c0_aL_der; // weight grad

					nng.connectionsList[layerL-1][connectionIndex] = c0_wL_der;
				}

				// calc bias gradient per layers
				float biasGrad = aL_zL_der * c0_aL_der;

				nng.biasesList[layerL - 1][neuronJ] = biasGrad;
			}
		}

		// add to accumulated gradient
		for (int c1 = 0; c1 < backProbNNG.connectionsList.size(); c1++)
		{
			// add to accumulated gradient
			for (int c2 = 0; c2 < backProbNNG.connectionsList[c1].size(); c2++)
			{
				backProbNNG.connectionsList[c1][c2] += nng.connectionsList[c1][c2];
			}
		}

		for (int c1 = 0; c1 < backProbNNG.biasesList.size(); c1++)
		{
			// add to accumulated gradient
			for (int c2 = 0; c2 < backProbNNG.biasesList[c1].size(); c2++)
			{
				backProbNNG.biasesList[c1][c2] += nng.biasesList[c1][c2];
			}
		}

	}

	// add to accumulated gradient
	// apply negative gradient to nn config
	for (int c1 = 0; c1 < backProbNNG.connectionsList.size(); c1++)
	{
		// add to accumulated gradient
		for (int c2 = 0; c2 < backProbNNG.connectionsList[c1].size(); c2++)
		{
			float connectionGradient = backProbNNG.connectionsList[c1][c2] / numberOfTrainingExamples;

			nn.connectionsList[c1][c2] -= connectionGradient;
		}
	}

	for (int c1 = 0; c1 < backProbNNG.biasesList.size(); c1++)
	{
		// add to accumulated gradient
		for (int c2 = 0; c2 < backProbNNG.biasesList[c1].size(); c2++)
		{
			float biasGradient = backProbNNG.biasesList[c1][c2] / numberOfTrainingExamples;

			nn.biasesList[c1][c2] -= biasGradient;
		}
	}
}

void NNGradient::Resize(const NNetwork& nn)
{
	connectionsList.resize(nn.connectionsList.size());
	for (int i = 0; i < connectionsList.size(); i++)
	{
		connectionsList[i].resize(nn.connectionsList[i].size());
	}
	biasesList.resize(nn.biasesList.size());
	for (int i = 0; i < biasesList.size(); i++)
	{
		biasesList[i].resize(nn.biasesList[i].size());
	}

}
