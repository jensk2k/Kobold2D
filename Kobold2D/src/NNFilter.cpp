#include "NNFilter.h"
#include "PrecisionStopWatch.h"
#include "Random.h"
#include <sstream>

#define res 300


NNFilter::NNFilter(Core& core)
	: GameState(core)
	, inputMap(res, res)
	, nn(0.1f, { 2, 16, 16, 3 })
	, nnDrawer(*this, nn)
{
	paths =
	{
		"assets/albums/teststar.png",
		"assets/albums/flagtest.png",
		"assets/albums/beatles1.jpg",
		"assets/albums/beatles2.jpg",
		"assets/albums/blink.jpg",
		"assets/albums/bowie1.jpg",
		"assets/albums/bowie2.jpg",
		"assets/albums/bruce.jpg",
		"assets/albums/clash.jpg",
		"assets/albums/cream.jpg",
		"assets/albums/darkside.png",
		"assets/albums/dolly.jpg",
		"assets/albums/elvis.jpg",
		"assets/albums/grace.jpg",
		"assets/albums/kate.jpg",
		"assets/albums/mac.jpg",
		"assets/albums/nirvana.jpg",
		"assets/albums/nwa.jpg",
		"assets/albums/pixies.jpg",
		"assets/albums/pj.jpg",
		"assets/albums/prince.jpg",
		"assets/albums/pumpkins.jpg",
		"assets/albums/strokes.jpg",
		"assets/albums/tribe.jpg",
		"assets/albums/velvet.jpg"
	};
}

void NNFilter::Init()
{
	LoadTexture(paths[0].c_str(), inputTexture, res, res);
	LoadImageMap(paths[0].c_str(), inputMap);
}

void NNFilter::Update()
{
	if (runBackProp)
	{
		PrecisionStopWatch sw;
		//int i = 0;
		//while (i < 500000)
		//{
		//	i++;
		while ((rtModeEnabled && sw.GetMilli() < 1.f / 20.f) || (!rtModeEnabled && backPropCount < maxBackProps))
		{
			Matrix input(1, 2);
			float x = Random::Range(0.f, 1.f);
			float y = Random::Range(0.f, 1.f);

			input(0, 0) = x;
			input(0, 1) = y;

			Matrix target(1, 3);
			Color colorSample = inputMap(x * inputMap.m_width, y * inputMap.m_height);

			target(0, 0) = colorSample.red / (float)255;
			target(0, 1) = colorSample.green / (float)255;
			target(0, 2) = colorSample.blue / (float)255;

			nn.BackPropagate(input, target);
			backPropCount++;

			sw.Stop();
		}

		if (testTimer > .1f)
		{
			testTimer = 0.f;

			RenderNNToTexture(outputTexture, 30, 30);

			float quickTest = RunQuickTests();
			lastTestScore = quickTest;

			if (quickTest > bestScore)
			{
				float fullTest = CalculateFitness();
				if (fullTest > bestScore)
				{
					bestScore = fullTest;
					bestLayerShapes = nn.layerShapes;
					RenderNNToTexture(bestTexture, 300, 300);
				}
			}

			histogram.RecordSample(quickTest);

			if (backPropCount >= quickTest * maxBackProps)
			{
				if (autoIncrementEnabled)
				{
					tryCount++;
					if (tryCount < maxTries)
					{
						Reset();
					}
					else
					{
						IncrementNNLayerShapes();
						tryCount = 0;
					}
				}
				else
				{
					Reset();
				}
			}
		}
		testTimer += GetDeltaTime();
	}

}

void NNFilter::Render()
{
	int margin = 10;
	//int imageDim = inputTexture.m_width * 2;
	int imageDim = (GetWindowWidth() / 3) - margin*1.5f;

	DrawTexture(inputTexture, Vec2i(margin, margin), imageDim, imageDim);

	{
		Vec2i pos(imageDim + margin * 2, margin);
		DrawTexture(outputTexture, pos, imageDim, imageDim);
	}
	{
		Vec2i pos(imageDim*2 + margin * 3, margin);
		DrawTexture(bestTexture, pos, imageDim, imageDim);
	}
	{
		std::ostringstream ss;
		ss << "Learning rate: " << nn.learningRate;
		ss << "\nCorrectness: " << 100.f * lastTestScore << "%";
		ss << "\nBest: " << 100.f * bestScore << "%";
		ss << "\nBest NN: ";
		for (int i = 0; i < bestLayerShapes.size(); i++)
		{
			ss << bestLayerShapes[i];
			if (i != bestLayerShapes.size() - 1)
				ss << ",";
		}
		ss << "\nBack Prop count: " << backPropCount;
		if (autoIncrementEnabled)
			ss << "\nTry count: " << tryCount+1 << "/" << maxTries;

		Vec2i textPos(margin, imageDim + margin * 2);

		DrawText(ss.str(), textPos.x, textPos.y);
	}
	int bottomYSpace = GetWindowHeight() - imageDim - margin * 2;
	{
		Vec2i pos(margin + 280, imageDim + margin*2);
		nnDrawer.DrawNeuralNetwork(nullptr, pos, 1000, bottomYSpace, false, 5);
	}
	{
		Vec2i pos(margin*2 + 280 + 1000, imageDim + margin * 2);
		histogram.Draw(*this, pos, bottomYSpace, nullptr, false, 1.f);
	}
}

void NNFilter::HandleKeyDown(Keys key)
{
	if (key == Keys::SPACE)
	{
		runBackProp = !runBackProp;
	}

	if (key == Keys::Q)
	{
		nn.SetLayerShapes({ 2, 3, 3 });
		Reset();
	}

	if (key == Keys::W)
	{
		IncrementNNLayerShapes();
		Reset();
	}

	if (key == Keys::E)
	{
		pathIndex++;
		if (pathIndex == paths.size())
			pathIndex = 0;
		LoadTexture(paths[pathIndex].c_str(), inputTexture, res, res);
		LoadImageMap(paths[pathIndex].c_str(), inputMap);
		bestScore = 0.f;
	}

	if (key == Keys::D)
	{
		pathIndex--;
		if (pathIndex == -1)
			pathIndex = paths.size()-1;
		LoadTexture(paths[pathIndex].c_str(), inputTexture, res, res);
		LoadImageMap(paths[pathIndex].c_str(), inputMap);
		bestScore = 0.f;
	}

	if (key == Keys::R)
	{
		std::cout << "Randomizing weights and biases" << std::endl;
		Reset();
	}	

	if (key == Keys::F)
	{
		bestScore = 0.f; 
		tryCount = 0;
	}

	if (key == Keys::S)
	{
		rtModeEnabled = !rtModeEnabled;
	}

	if (key == Keys::Z)
	{
		autoIncrementEnabled = !autoIncrementEnabled;
	}
}

void NNFilter::RenderNNToTexture(Texture& texture, unsigned width, unsigned height)
{
	Map2D<Color> map(width, height);
	for (int y = 0; y < map.m_height; y++)
	{
		for (int x = 0; x < map.m_width; x++)
		{
			float xGrad = x / (float)map.m_width;
			float yGrad = y / (float)map.m_height;

			Matrix input(1, 2, { xGrad, yGrad });
			Matrix output = nn.Solve(input);

			Color c(255 * output(0, 0), 255 * output(0, 1), 255 * output(0, 2));

			map(x, y) = c;
		}
	}
	RenderPixelMapToTexture(map, texture);
}

Matrix NNFilter::GetTarget(Matrix input) const
{
	float x = input(0, 0);
	float y = input(0, 1);

	Matrix target(1, 3);
	Color colorSample = inputMap(x * inputMap.m_width, y * inputMap.m_height);

	target(0, 0) = colorSample.red / (float)255;
	target(0, 1) = colorSample.green / (float)255;
	target(0, 2) = colorSample.blue / (float)255;

	return target;
}

float NNFilter::RunQuickTests() const
{
	float correctness = 0.f;
	for (int i = 0; i < 1000; i++)
	{
		int x = Random::Range(0, inputMap.m_width);
		int y = Random::Range(0, inputMap.m_height);

		Matrix testInput(1, 2, { x / (float)inputMap.m_width, y / (float)inputMap.m_height });
		Matrix target = GetTarget(testInput);
		Matrix output = nn.Solve(testInput);

		float localCorrectness = 1.f;

		for (int i = 0; i < output.Rows(); i++)
		{
			float error = MathUtils::Abs(target(0, i) - output(0, i));

			localCorrectness *= 1.f - error;
		}
		correctness += localCorrectness;
	}

	int trainingDataSize = 1000;

	return correctness / (float)trainingDataSize;
}

float NNFilter::CalculateFitness() const
{
	float correctness = 0.f;
	for (int y = 0; y < inputMap.m_height; y++)
	{
		for (int x = 0; x < inputMap.m_width; x++)
		{

			Matrix testInput(1, 2, {x / (float)inputMap.m_width, y / (float)inputMap.m_height });
			Matrix target = GetTarget(testInput);
			Matrix output = nn.Solve(testInput);
			
			float localCorrectness = 1.f;

			for (int i = 0; i < output.Rows(); i++)
			{
				float error = MathUtils::Abs(target(0, i) - output(0, i));

				localCorrectness *= 1.f - error;
			}
			correctness += localCorrectness;
		}
	}

	int trainingDataSize = inputMap.m_height * inputMap.m_width;

	return correctness / trainingDataSize;
}

void NNFilter::Reset()
{
	nn.RandomizeWeightsAndBiases();
	histogram.Reset();

	backPropCount = 0;
	lastTestScore = 0.f;
	//bestScore = 0.f;
	testTimer = 0.f;
	hiddenLayerIndex = 0;
	//tryCount = 0;
}

void NNFilter::IncrementNNLayerShapes()
{
	static int minNeuronsPerLayer = 3;
	static int maxNeuronsPerLayer = 20;

	std::vector<unsigned> layerShapes{2};
	int nHiddenLayers = nn.layerShapes.size() - 2;

	int shape = nn.layerShapes[hiddenLayerIndex + 1];


	bool needNewLayer = true;

	for (int i = 0; i < nHiddenLayers; i++)
	{
		int foo = nn.layerShapes[i + 1];
		if (foo < maxNeuronsPerLayer)
			needNewLayer = false;
	}

	if (!needNewLayer)
	{
		if (shape < maxNeuronsPerLayer)
		{
			for (int i = 0; i < nHiddenLayers; i++)
			{
				int foo = nn.layerShapes[i + 1];
				if (i == hiddenLayerIndex)
					foo++;
				layerShapes.push_back(foo);
			}
		}
		else
		{
			while (nn.layerShapes[hiddenLayerIndex + 1] == maxNeuronsPerLayer)
			{
				hiddenLayerIndex++;
			}
			for (int i = 0; i < nHiddenLayers; i++)
			{
				int foo = nn.layerShapes[i + 1];
				if (i <= hiddenLayerIndex - 1)
				{
					foo = minNeuronsPerLayer;
				}
				else if (i == hiddenLayerIndex)
				{
					foo++;
				}

				layerShapes.push_back(foo);
			}
			hiddenLayerIndex = 0;
		}
	}
	else
	{
		nHiddenLayers++;
		hiddenLayerIndex = 0;

		for (int i = 0; i < nHiddenLayers; i++)
		{
			layerShapes.push_back(minNeuronsPerLayer);
		}
	}

	layerShapes.push_back(3);

	nn.SetLayerShapes(layerShapes);
}
