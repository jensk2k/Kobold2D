#include "GAMetaBalls.h"
#include "Random.h"
#include "MathUtils.h"
#include "PrecisionStopWatch.h"

#include <sstream>

static float CalculateFitness(Map2D<Color>& sourceMap, Map2D<Color>& targetMap)
{
	float correctness = 0.f;
	for (int y = 0; y < targetMap.m_height; y++)
	{
		for (int x = 0; x < targetMap.m_width; x++)
		{
			int x2 = ((float)x / targetMap.m_width) * sourceMap.m_width;
			int y2 = ((float)y / targetMap.m_height) * sourceMap.m_height;

			Color target = targetMap(x, y);
			Color source = sourceMap(x2, y2);

			float r = 1.f - (MathUtils::Abs(target.red - source.red) / 255);
			float g = 1.f - (MathUtils::Abs(target.green - source.green) / 255);
			float b = 1.f - (MathUtils::Abs(target.blue - source.blue) / 255);
			//r = r < 0.9f ? 0.f : r;
			//g = g < 0.9f ? 0.f : g;
			//b = b < 0.9f ? 0.f : b;

			float localCorrectness = r * g * b;
			////if (localCorrectness < 0.8f)
			////	localCorrectness = 0.f;
			//localCorrectness = pow(localCorrectness, 10.f);
			correctness += localCorrectness;
			//bool r = target.red == source.red;
			//bool g = target.green == source.green;
			//bool b = target.blue == source.blue;
			//correctness += r && g && b;
		}
	}

	int trainingDataSize = targetMap.m_height * targetMap.m_width;
	correctness /= trainingDataSize;
	//correctness = correctness * correctness;

	return correctness;
}

static float CalculateFitnessFast(Map2D<Color>& sourceMap, Map2D<Color>& targetMap, int samples)
{
	float correctness = 0.f;
	for (int i = 0; i < samples; i++)
	{
		int x = Random::Range(0, sourceMap.m_width);
		int y = Random::Range(0, sourceMap.m_height);

		int x2 = ((float)x / sourceMap.m_width) * targetMap.m_width;
		int y2 = ((float)y / sourceMap.m_height) * targetMap.m_height;

		Color source = sourceMap(x, y);
		Color target = targetMap(x2, y2);

		int redError = MathUtils::Abs(target.red - source.red);
		int greenError = MathUtils::Abs(target.green - source.green);
		int blueError = MathUtils::Abs(target.blue - source.blue);

		float error = (float)(redError + greenError + blueError) / (255.f + 255.f + 255.f);
		if (error > 0.05f)
			error = 1.f;

		correctness += 1.f - error;

		//float r = 1.f - MathUtils::Abs(target.red - source.red) / 255;
		//float g = 1.f - MathUtils::Abs(target.green - source.green) / 255;
		//float b = 1.f - MathUtils::Abs(target.blue - source.blue) / 255;

		//float localCorrectness = r * g * b;
		//correctness += localCorrectness;

		//bool r = target.red == source.red;
		//bool g = target.green == source.green;
		//bool b = target.blue == source.blue;
		//correctness += r && g && b;
	}

	return correctness / samples;
}

static void GenerateRandomColors(int n, std::vector<Color>& colors)
{
	colors.resize(n);
	for (int i = 0; i < n; i++)
	{
		colors[i] = Color(Random::Range(0, 255), Random::Range(0, 255), Random::Range(0, 255));
	}
}

static void GenerateRandomBlobs(int amount, std::vector<NNBlob>& blobs)
{
	blobs.resize(amount);
	for (int i = 0; i < amount; i++)
	{
		NNBlob& b = blobs[i];
		b.pos = Vec2f(Random::Range(0.f, 1.f), Random::Range(0.f, 1.f));
		b.radius = Random::Range(0.f, BLOB_MAX_RADIUS) * Random::Range(0.f, BLOB_MAX_RADIUS);
	}
}

static int GetWeightedRandom(std::vector<float>& numbers)
{
	float total = 0.f;
	for (float f : numbers)
		total += f;
	float random = Random::Range(0.f, 1.f);
	float accumulatedPropability = 0.f;
	for (int i = 0; i < numbers.size(); i++)
	{
		float elementPropability = numbers[i] / total;
		accumulatedPropability += elementPropability;
		if (random < accumulatedPropability)
			return i;
	}
	return -1;
}

static void CrossOver(const Gene& specimen1, const Gene& specimen2, Gene& child)
{
	child.colors.resize(specimen1.colors.size());
	child.blobs.resize(specimen1.blobs.size());
	int range = specimen1.colors.size() + specimen1.blobs.size() - 1;
	int rand = Random::Range(0, range);
	if (rand < specimen1.colors.size() - 1)
	{
		int colorCrossIndex = rand;

		for (int i = 0; i < specimen1.colors.size(); i++)
		{
			if (colorCrossIndex <= i)
				child.colors[i] = specimen1.colors[i];
			else
				child.colors[i] = specimen2.colors[i];
		}
		child.blobs = specimen2.blobs;
	}
	else
	{
		int blobsCrossIndex = rand - specimen1.colors.size() - 1;

		for (int i = 0; i < specimen1.blobs.size(); i++)
		{
			if (blobsCrossIndex <= i)
				child.blobs[i] = specimen1.blobs[i];
			else
				child.blobs[i] = specimen2.blobs[i];
		}
		child.colors = specimen1.colors;
	}
}

static void Mutate(Gene& child)
{
	int range = child.colors.size() + child.blobs.size() - 1;
	int rand = Random::Range(0, range);

	if (rand < child.colors.size())
	{
		int colorMutationIndex = rand;
		//child.colors[colorMutationIndex] = Color(Random::Range(0, 255), Random::Range(0, 255), Random::Range(0, 255));
		Color curColor = child.colors[colorMutationIndex];
		child.colors[colorMutationIndex] = Color(curColor.red + Random::Range(-4,4), curColor.green + Random::Range(-4, 4), curColor.blue + Random::Range(-4, 4));
	}
	else
	{
		int blobsMutationIndex = rand - child.colors.size();
		float rand2 = Random::Range(0.f, 1.f);
		//if (rand2 < 0.333f)
		//	child.blobs[blobsMutationIndex].pos.x = Random::Range(0.f, 1.f);
		//else if (rand2 < 0.667f)
		//	child.blobs[blobsMutationIndex].pos.y = Random::Range(0.f, 1.f);
		//else
		//	child.blobs[blobsMutationIndex].radius = Random::Range(0.f, BLOB_MAX_RADIUS) * Random::Range(0.f, BLOB_MAX_RADIUS);
		if (rand2 < 0.333f)
			child.blobs[blobsMutationIndex].pos.x += Random::Range(-.1f, .1f);
		else if (rand2 < 0.667f)
			child.blobs[blobsMutationIndex].pos.y += Random::Range(-.1f, .1f);
		else
			child.blobs[blobsMutationIndex].radius += Random::Range(-.1f, .1f);

		child.blobs[blobsMutationIndex].pos.x = MathUtils::Clamp(child.blobs[blobsMutationIndex].pos.x, 0, 1.f);
		child.blobs[blobsMutationIndex].pos.y = MathUtils::Clamp(child.blobs[blobsMutationIndex].pos.y, 0, 1.f);
		child.blobs[blobsMutationIndex].radius = MathUtils::Clamp(child.blobs[blobsMutationIndex].radius, 0, BLOB_MAX_RADIUS);

	}
}

GAMetaBalls::GAMetaBalls(Core& core)
	: GameState(core)
{
	paths =
	{
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

void GAMetaBalls::Init()
{
	LoadInputImage(0);
}

void GAMetaBalls::Update()
{
	if (running)
	{
		Iterate();
	}
}

void GAMetaBalls::Render()
{
	int dim = GetWindowWidth() / 3;

	DrawTexture(inputTex, Vec2i(0, 0), dim, dim);
	DrawTexture(outputTex, Vec2i(dim, 0), dim, dim);
	DrawTexture(bestTex, Vec2i(dim*2, 0), dim, dim);

	{
		std::ostringstream ss;
		ss << "Generation: " << generation;
		ss << "\nPopulation: " << population.size();
		ss << "\nMutation Rate: " << MUTATION_RATE * 100 << "%";
		ss << "\nAverage Fitness: " << averageFitness * 100 << "%";
		ss << "\nHighest Fitness: " << highestFitness * 100 << "%";

		Vec2i textPos(5, dim + 5);

		DrawText(ss.str(), textPos.x, textPos.y);
	}
	{
		Vec2i pos(300, dim + 5);
		int size = GetWindowHeight() - dim - 10;
		histogram.Draw(*this, pos, size, nullptr, false, 1.f);
	}
}

void GAMetaBalls::HandleKeyDown(Keys key)
{
	if (key == Keys::Q)
	{
		InitializePopulation();
	}

	if (key == Keys::R)
	{
		inputImageIndex++;
		if (inputImageIndex == paths.size())
			inputImageIndex = 0;
		LoadInputImage(inputImageIndex);
		Reset();
	}

	if (key == Keys::E)
	{
		inputImageIndex--;
		if (inputImageIndex == -1)
			inputImageIndex = paths.size() - 1;
		LoadInputImage(inputImageIndex);
		Reset();
	}
	if (key == Keys::SPACE)
	{
		running = !running;
		if (population.size() == 0)
		{
			InitializePopulation();
		}
	}
}

void GAMetaBalls::InitializePopulation()
{
	PrecisionStopWatch sw;
	population.resize(POPULATION);
	for (int i = 0; i < population.size(); i++)
	{
		Gene& g = population[i];
		GenerateRandomBlobs(10, g.blobs);
		GenerateRandomColors(4, g.colors);
	}

	RecalculateFitness();

	RenderBlobs(population[bestSpecimen].blobs, population[bestSpecimen].colors, outputMap, inputMap.m_width);

	RenderPixelMapToTexture(outputMap, outputTex);
	sw.Stop();
	std::cout << "Initialized population - " << sw.GetMilli() << " ms" << std::endl;
}

void GAMetaBalls::Iterate()
{
	PrecisionStopWatch sw;
	std::vector<Gene> newPopulation(population.size());
	int count = 0;
	while (count < population.size())
	{
		int valids = 0;
		for (int i = 0; i < population.size(); i++)
			if (fitnesses[i] > 0.f)
				valids++;
		if (valids < 2)
		{
			InitializePopulation();
			continue;
		}

		//	Selection
		int parent1 = GetWeightedRandom(fitnesses);
		int parent2 = GetWeightedRandom(fitnesses);

		if (parent1 == parent2)
			continue;
		//	Crossover
		Gene& child = newPopulation[count];
		CrossOver(population[parent1], population[parent2], child);

		//	Mutation
		if (Random::Range(0.f, 1.f) < MUTATION_RATE)
		{
			Mutate(child);
			//std::cout << "MUTATION" << std::endl;
		}
		count++;
	}
	population = newPopulation;
	generation++;

	//PrecisionStopWatch sw2;
	//sw2.Stop();
	//std::cout << "Generating new population took " << sw2.GetMilli() << " ms" << std::endl;
	
	//sw2.Start();
	RecalculateFitness();
	//sw2.Stop();
	//std::cout << "Recalculating fitness took " << sw2.GetMilli() << " ms" << std::endl;


	//sw2.Start();
	RenderBlobs(population[bestSpecimen].blobs, population[bestSpecimen].colors, outputMap, inputMap.m_width);
	//sw2.Stop();
	//std::cout << "Rendering best took " << sw2.GetMilli() << " ms" << std::endl;

	//sw2.Start();
	RenderPixelMapToTexture(outputMap, outputTex);
	//sw2.Stop();
	//std::cout << "Rendering texture took " << sw2.GetMilli() << " ms" << std::endl;

	if (highestFitness > allTimeBest)
	{
		allTimeBest = highestFitness;
		bestMap = outputMap;
		RenderPixelMapToTexture(bestMap, bestTex);
	}

	sw.Stop();
	std::cout << "Total iteration took " << sw.GetMilli() << " ms" << std::endl;
}

void GAMetaBalls::RecalculateFitness()
{
	PrecisionStopWatch sw;
	fitnesses.resize(population.size());

	Map2D<Color> map;
	for (int i = 0; i < population.size(); i++)
	{
		Gene& g = population[i];

		RenderBlobs(g.blobs, g.colors, map, inputMap.m_width);

		fitnesses[i] = CalculateFitness(inputMap, map);
 		//fitnesses[i] = CalculateFitnessFast(inputMap, map, 100);
	}

	highestFitness = 0.f;
	float totalFitness = 0.f;

	for (int i = 0; i < fitnesses.size(); i++)
	{
		totalFitness += fitnesses[i];
		if (fitnesses[i] > highestFitness)
		{
			bestSpecimen = i;
			highestFitness = fitnesses[i];
		}
	}
	averageFitness = totalFitness / fitnesses.size();

	histogram.RecordSample(highestFitness);
	sw.Stop();
	std::cout << "Recalculating fitness took " << sw.GetMilli() << " ms" << std::endl;
}

void GAMetaBalls::Reset()
{
	population.clear();
	fitnesses.clear();

	averageFitness = 0.f;
	highestFitness = 0.f;
	bestSpecimen = 0;
	allTimeBest = 0.f;

	generation = 0;
	running = false;

	histogram.Reset();
}

void GAMetaBalls::RenderBlobs(const std::vector<NNBlob> blobs, const std::vector<Color>& colors, Map2D<Color>& colorMap, int dimensions) const
{
	colorMap = Map2D<Color>(dimensions, dimensions);

	for (int y = 0; y < colorMap.m_height; y++)
	{
		for (int x = 0; x < colorMap.m_width; x++)
		{
			float sum = 0.f;

			for (int i = 0; i < blobs.size(); i++)
			{
				const NNBlob& blob = blobs[i];
				float radius = blob.radius;

				Vec2f coord((float)x / colorMap.m_width, (float)y / colorMap.m_height);
				float distance = (coord - blob.pos).Length();

				float val = distance > 0.f ? radius / distance : 1.f;

				val = val * val;

				sum += val;

			}
			sum = MathUtils::Clamp(sum, 0.f, 1.f);
			
			Color c;
			{
				if (sum < 1.f)
				{
					int index = (colors.size() - 1.f) * sum;
					Color c1 = colors[index];
					Color c2 = colors[index + 1];
					float interpolation = (colors.size() - 1.f) * sum - index;
					c = Color::Lerp(c1, c2, interpolation);
				}
				else
					c = colors[colors.size() - 1];
			}

			colorMap(x, y) = c;
		}
	}

}

void GAMetaBalls::LoadInputImage(int index)
{
	LoadImageMap(paths[index].c_str(), inputMap);

	LoadTexture(paths[index].c_str(), inputTex, inputMap.m_width, inputMap.m_height);
}
