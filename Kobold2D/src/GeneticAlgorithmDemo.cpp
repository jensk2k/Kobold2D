#include "GeneticAlgorithmDemo.h"
#include "Random.h"
#include <sstream>

static std::string possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

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
	return Random::Range(0, numbers.size() - 1);
}

GeneticAlgorithmDemo::GeneticAlgorithmDemo(Core& core)
	: GameState(core)
	, target("WHAT COULD GO WRONG")
{}

void GeneticAlgorithmDemo::Update()
{
	if (isRunning)
	{
		Iterate();

		//if (highestFitness > .999f)
		if (target.compare(best) == 0)
			isRunning = false;
	}
}

void GeneticAlgorithmDemo::Render()
{
	std::ostringstream os;
	os << "Generation: " << generation << std::endl;
	os << "Target: " << target << std::endl;
	os << "Highest Fitness: " << (int)(ceil(highestFitness*100.f)) << "%" << std::endl << std::endl;
	os << best << std::endl;
	DrawText(os.str(), 0,0);


	int j = 0;
	int columns = 5;
	for (int i = 0; i < columns; i++)
	{
		std::ostringstream os;
		//os << "Population:";
		//for (int j = i * population.size() / 4; j < i+1 * population.size() / 4; j++)
		int k = 0;
		while(k < ceil(population.size() / (float)columns))
		{
			if (j == population.size())
				break;
			os << j << " " << population[j] << "\n";
			j++;
			k++;
		}
		DrawText(os.str(), GetWindowWidth() / 5 + i * 300, 0);
	}
}

void GeneticAlgorithmDemo::HandleKeyDown(Keys key)
{
	if (key == Keys::SPACE)
	{
		isRunning = !isRunning;

		if (population.size() == 0 || target.compare(best) == 0)
			Initialize();
	}
	if (key == Keys::I)
	{
		isRunning = false;

		if (population.size() == 0 || target.compare(best) == 0)
			Initialize();

		Iterate();
	}
}

void GeneticAlgorithmDemo::CalcFitness()
{
	fitnesses.resize(population.size());
	int targetLength = target.length();
	for (int i = 0; i < population.size(); i++)
	{
		float fitness = 0.f;
		for (int j = 0; j < targetLength; j++)
		{
			if (target[j] == population[i][j])
				fitness += 1.f / targetLength;
		}
		fitnesses[i] = fitness;
	}
}

void GeneticAlgorithmDemo::Initialize()
{
	// initialize population
	int targetLength = target.length();
	generation = 0;
	highestFitness = 0.f;
	population.resize(POPULATION);

	for (int i = 0; i < population.size(); i++)
	{
		std::string& s = population[i];
		s = "";
		for (int j = 0; j < targetLength; j++)
		{
			//char randChar = (char)i;
			char randChar = (char)Random::Range(0, 255);
			//s += randChar;
			s += possibleCharacters[Random::Range(0, possibleCharacters.length())];
		}
	}
	CalcFitness();
}

void GeneticAlgorithmDemo::Iterate()
{
	std::vector<std::string> newPopulation(population.size());
	int count = 0;
	int targetLength = target.length();
	while (count < population.size())
	{
		//	Selection
		int parent1 = GetWeightedRandom(fitnesses);
		int parent2 = GetWeightedRandom(fitnesses);

		if (parent1 == parent2)
			continue;
		std::string& child = newPopulation[count];
		child.reserve(targetLength);

		//	Crossover
		int crossOverIndex = Random::Range(0, targetLength);
		for (int i = 0; i < targetLength; i++)
		{
			if (i < crossOverIndex)
				child += population[parent1][i];
			else
				child += population[parent2][i];
		}

		//	Mutation
		if (Random::Range(0.f, 1.f) < MUTATION_RATE)
		{
			int mutationIndex = Random::Range(0, targetLength);
			child[mutationIndex] = possibleCharacters[Random::Range(0, possibleCharacters.length())];
		}
		count++;
	}
	population = newPopulation;
	generation++;

	// recalc fitness
	CalcFitness();

	highestFitness = 0.f;
	float totalFitness = 0.f;
	int bestIndex = -1;

	for (int i = 0; i < fitnesses.size(); i++)
	{
		totalFitness += fitnesses[i];
		if (fitnesses[i] > highestFitness)
		{
			bestIndex = i;
			highestFitness = fitnesses[i];
		}
	}
	float averageFitness = totalFitness / fitnesses.size();
	best = population[bestIndex];
}
