#include "NeuralNetworkDemo.h"
#include "Random.h"
#include "PrecisionStopWatch.h"
#include <sstream>
#include <iomanip>

static Matrix RandomInput(unsigned rows)
{
	std::vector<float> iniData;
	for (unsigned i = 0; i < rows; i++)
		iniData.push_back(Random::Range(0.f, 1.f));
	return Matrix(1, rows, iniData);
}

static Matrix GetCorrectXOROutput(const Matrix& input)
{
	Matrix output(1, 1);
	output(0, 0) = input(0, 0) != input(0, 1);
	return output;
}

NeuralNetworkDemo::NeuralNetworkDemo(Core& core)
	: GameState(core)
	, nn(0.1f, { 2, 2, 1 })
	, colorMap(28, 28)
{
	trainingData = 
	{
		Matrix(1,2, { 0.f, 1.f }),
		Matrix(1,2, { 1.f, 0.f }),
		Matrix(1,2, { 0.f, 0.f }),
		Matrix(1,2, { 1.f, 1.f }),
	};

	int randomIndex = Random::Range(0, trainingData.size());
	displayedInput = trainingData[randomIndex];
}

void NeuralNetworkDemo::Update() 
{
	if (runAnimBackProp)
	{
		PrecisionStopWatch sw;
		while (sw.GetMilli() < 1.f / 30.f)
		{
			int randomIndex = Random::Range(0, trainingData.size());
			displayedInput = trainingData[randomIndex];
			Matrix target = GetCorrectXOROutput(displayedInput);
			nn.BackPropagate(displayedInput, target);
			backPropCount++;
			backPropsPerSecondCounter++;

			RunTests();
			sw.Stop();
		}

		histogram.RecordSample(lastTestScore);

		if (backPropCount >= lastTestScore*100000)
		{
			Reset();
		}
	}
}

void NeuralNetworkDemo::Render()
{
	int margin = 10;
	Vec2i topLeft(margin, margin);
	int width = GetWindowWidth() - margin * 2;
	int height = GetWindowHeight() - margin * 2 - (GetWindowHeight() / 3);


	int bottomYSpace = GetWindowHeight() - height - 3.f * margin;

	DrawNeuralNetwork(nn, topLeft, width, height);
		
	{
		for (int y = 0; y < colorMap.m_height; y++)
		{
			for (int x = 0; x < colorMap.m_width; x++)
			{
				float xGrad = x / (float)colorMap.m_width;
				float yGrad = y / (float)colorMap.m_height;

				Matrix input(1, 2, { xGrad, yGrad });
				Matrix output = nn.Solve(input);

				float val = output(0, 0);

				//Color c(255 * val, 255 * val, 255 * val);
				Color c = InterpolateColor(Colors::BLACK, Colors::WHITE, val);
				//Color col1 = Colors::CYAN;
				//Color col2 = Colors::MAGENTA;
				//Color col3 = Colors::YELLOW;
				//Color c = val < 0.5f ? InterpolateColor(col1, col2, val * 2.f) : InterpolateColor(col2, col3, (val - 0.5f) * 2.0f);

				colorMap(x, y) = c;
			}
		}
		RenderPixelMapToTexture(colorMap, tex);

		Vec2i pos(margin, topLeft.y + height + margin);
		DrawTexture(tex, pos, bottomYSpace, bottomYSpace);
	}
	{
		Vec2i pos(bottomYSpace + margin * 2, topLeft.y + height + margin);

		histogram.Draw(*this, pos, bottomYSpace, nullptr, false, 1.f);
	}
	{
		backPropsPerSecondTimer += GetDeltaTime();
		//backPropsPerFrameCounter++;
		if (backPropsPerSecondTimer > .5f)
		{
			backPropsPerSecond = backPropsPerSecondCounter / backPropsPerSecondTimer;
			backPropsPerSecondCounter = 0;
			backPropsPerSecondTimer = 0.f;
		}

		std::ostringstream ss;
		ss << "Learning rate: " << nn.learningRate;
		ss << "\nCorrectness: " << 100.f * lastTestScore << "%";
		ss << "\nBest: " << 100.f * bestScore << "%";
		ss << "\nBack Prop count: " << backPropCount;
		ss << "\nBack Props per second: " << backPropsPerSecond;

		Vec2i textPos(bottomYSpace*2 + margin*5, topLeft.y + height + margin*2);

		DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
	}
}

void NeuralNetworkDemo::DrawNeuralNetwork(const NeuralNetwork& nn, Vec2i topLeft, int width, int height)
{
	int neuronRadius = 20;

	DrawRectangle(topLeft, width, height, Colors::WHITE);

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
				Color c = InterpolateColor(Colors::RED, Colors::BLUE, sigW);

				DrawLine(neuronPos1 + offset, neuronPos2 - offset, c);

				std::ostringstream ss;
				ss << (int)(10 * weight) / 10.f;

				Vec2i textPos = neuronPos1 + ((neuronPos2 - neuronPos1) / 4);

				DrawText(ss.str(), textPos.x - 16, textPos.y - 13);
			}
		}
	}
	
	std::vector<Matrix> activations;
	nn.CalcActivations(displayedInput, activations);

	// calc errors
	Matrix targets = GetCorrectXOROutput(displayedInput);

	// Draw nodes
	for (int i = 0; i < nn.layerShapes.size(); i++)
	{
		Matrix activationsLayer = activations[i]; 

		// neuron
		for (unsigned j = 0; j < nn.layerShapes[i]; j++)
		{
			float neuronActivaion = activationsLayer(0, j);

			Vec2i centerPos = GetNeuronDrawPosition(topLeft, width, height, nn, i, j);

			DrawCircleSolid(centerPos, neuronRadius, Color(255 * neuronActivaion, 255 * neuronActivaion, 255 * neuronActivaion));
			DrawCircle(centerPos, neuronRadius, Colors::WHITE);

			if (i == 0)
				continue;
			{
				float bias = nn.biases[i - 1](0, j);

				std::ostringstream ss;
				ss << (int)(10 * bias) / 10.f;

				DrawText(ss.str(), centerPos.x - 16, centerPos.y - 13 - 32);
			}
		}
	}
}

Vec2i NeuralNetworkDemo::GetNeuronDrawPosition(Vec2i topLeft, int rectWidth, int rectHeight, const NeuralNetwork& nn, int layerIndex, int neuronIndex)
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

void NeuralNetworkDemo::HandleKeyDown(Keys key)
{
	if (key == Keys::SPACE)
	{
		runAnimBackProp = !runAnimBackProp;
	}

	if (key == Keys::Q)
	{
		std::cout << "Randomizing displayed input" << std::endl;
		int randomIndex = Random::Range(0, trainingData.size());
		displayedInput = trainingData[randomIndex];

		Matrix output = nn.Solve(displayedInput);
		std::cout << displayedInput << " -> " << output << std::endl;
	}

	if (key == Keys::R)
	{
		std::cout << "Randomizing weights and biases" << std::endl;
		Reset();
		RunTests();
	}

	if (key == Keys::T)
	{
		std::cout << "Testing..." << std::endl;
		RunTests();
		std::cout << "Correctness: " << lastTestScore << "%" << std::endl;
	}

	if (key == Keys::B)
	{
		std::cout << "Backprop..." << std::endl;

		for (int i = 0; i < 50000; i++)
		{
			int randomIndex = Random::Range(0, trainingData.size());

			Matrix input = trainingData[randomIndex];
			Matrix targets = GetCorrectXOROutput(input); // replace with some sort of callback to a training data retriever

			nn.BackPropagate(input, targets);
		}

		std::cout << "Backprop Done" << std::endl;
	}
}

void NeuralNetworkDemo::RunTests()
{
	float correctness = 0.f;
	for (Matrix test : trainingData)
	{
		float target = GetCorrectXOROutput(test)(0, 0);
		float output = nn.Solve(test)(0, 0);

		correctness += 1.f - MathUtils::Abs(target - output);
	}
	lastTestScore = ((int)((correctness / (float)trainingData.size())*10000.f))/10000.f;

	if (lastTestScore > bestScore)
		bestScore = lastTestScore;
}

void NeuralNetworkDemo::Reset()
{
	nn.RandomizeWeightsAndBiases();
	backPropCount = 0;
	histogram.Reset();
}

