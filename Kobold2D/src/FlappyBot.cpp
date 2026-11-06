#include "FlappyBot.h"
#include "Random.h"
#include <sstream>
#include <optional>
#include "PrecisionStopWatch.h"

static void RandomizeLayerShapes(std::vector<unsigned>& layerShapes)
{
	layerShapes = { 5, 5, 1 };

	//int nHiddenLayers = Random::Range(1, 3);
	//layerShapes.resize(nHiddenLayers + 2);
	//layerShapes[0] = 5;
	//for (int i = 1; i < nHiddenLayers + 1; i++)
	//	layerShapes[i] = Random::Range(1, 8);

	//layerShapes[nHiddenLayers + 1] = 1;

	//for (unsigned layer : layerShapes)
	//	std::cout << layer << " ";
	//std::cout << std::endl;
}

static int GetWeightedRandom(std::array<float, POPULATION>& numbers)
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

static int GetBest(std::array<float, POPULATION>& numbers)
{
	int bestIndex = -1;
	float bestScore = 0.f;

	for (int i = 0; i < numbers.size(); i++)
	{
		if (numbers[i] > bestScore)
		{
			bestScore = numbers[i];
			bestIndex = i;
		}
	}
	return bestIndex;
}

static void CrossOver(const NeuralNetwork& parent1, const NeuralNetwork& parent2, NeuralNetwork& child)
{
	//child.SetLayerShapes(parent1.layerShapes);
	//child.weights.resize(parent1.weights.size());
	//child.biases.resize(parent1.biases.size());

	for (int layer = 0; layer < parent1.weights.size(); layer++)
	{
		for (int row = 0; row < parent1.weights[layer].Rows(); row++)
		{
			for (int col = 0; col < parent1.weights[layer].Columns(); col++)
			{
				float weight = Random::Range(0.f, 1.f) < 0.5f ?
					parent1.weights[layer](col, row) :
					parent2.weights[layer](col, row);
				//child.weights[layer] = Matrix(parent1.weights[layer].Columns(), parent1.weights[layer].Rows());
				child.weights[layer](col, row) = weight;
			}
		}
	}
	for (int layer = 0; layer < parent1.biases.size(); layer++)
	{
		for (int row = 0; row < parent1.biases[layer].Rows(); row++)
		{
			float bias = Random::Range(0.f, 1.f) < 0.5f ? 
				parent1.biases[layer](0, row) :
				parent2.biases[layer](0, row);
			//child.biases[layer] = Matrix(parent1.biases[layer].Columns(), parent1.biases[layer].Rows());
			child.biases[layer](0, row) = bias;
		}
	}
}

static void Mutate(NeuralNetwork& child)
{
	for (int layer = 0; layer < child.weights.size(); layer++)
	{
		for (int row = 0; row < child.weights[layer].Rows(); row++)
		{
			for (int col = 0; col < child.weights[layer].Columns(); col++)
			{
				if (Random::Range(0.f, 1.f) < MUTATION_RATE)
				{
					float offset = Random::Range(-1.f, 1.f) * Random::Range(-1.f, 1.f)*0.5f;
					child.weights[layer](col, row) += offset;
				}
			}
		}
	}

	for (int layer = 0; layer < child.biases.size(); layer++)
	{
		for (int row = 0; row < child.biases[layer].Rows(); row++)
		{
			if (Random::Range(0.f, 1.f) < MUTATION_RATE)
			{
				float offset = Random::Range(-1.f, 1.f) * Random::Range(-1.f, 1.f) * 0.5f;
				child.biases[layer](0, row) += offset;
			}
		}
	}
}


void FlappyBot::Init()
{	
	pipes[0].upperPhysicsID = physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(), PIPE_WIDTH, 0.f);
	pipes[0].lowerPhysicsID = physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(), PIPE_WIDTH, 0.f);

	pipes[1].upperPhysicsID = physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(), PIPE_WIDTH, 0.f);
	pipes[1].lowerPhysicsID = physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(), PIPE_WIDTH, 0.f);

	for (int i = 0; i < POPULATION; i++)
	{
		birds[i] = { -BIRD_SIZE / 2.f, 0.f };
		birds[i].physicsID = physicsWorld.AddBox(Physics::Layer::Agent, Vec2f(), BIRD_SIZE, BIRD_SIZE);
	}

	ResetGame();
	if (isBotModeEnabled)
		ResetBots();

	histogram.cap = 200;
}

void FlappyBot::HandleKeyDown(Keys key)
{
	if (key == Keys::B)
	{
		highscore = 0;
		highscoreGeneration = 0;

		isPlaying = false;
		isBotModeEnabled = !isBotModeEnabled;
		ResetGame();
		if (isBotModeEnabled)
			ResetBots();
	}

	if (key == Keys::R)
	{
		isPlaying = false;

		if (isBotModeEnabled)
			ResetBots();
		ResetGame();
	}
	
	if (key == Keys::SPACE)
	{
		if (!isBotModeEnabled)
		{
			if (isPlaying)
			{
				Flap(birds[0]);
			}
			else
			{
				isPlaying = true;
				ResetGame();
			}
		}
		else
		{
			if (!isPlaying)
			{
				isPlaying = true;
				ResetGame();
			}
			else
			{
				isTrainingModeEnabled = !isTrainingModeEnabled;
			}
		}
	}
}

void FlappyBot::Update()
{
	viewport.scale = GetWindowHeight() / 36.f;

	if (!isPlaying)
		return;

	PrecisionStopWatch psw;
	float timePassed;

	do
	{
		float scaledDeltaTime = isTrainingModeEnabled && isBotModeEnabled ? 1.f / 60.f : GetDeltaTime();

		if (isBotModeEnabled)
			globalFitness += scaledDeltaTime;

		if (isBotModeEnabled)
		{
			for (int i = 0; i < POPULATION; i++)
			{
				Bird& bird = birds[i];
				if (!bird.alive)
					break;

				Matrix input = GenNNInput(bird);

				Matrix output = bots[i].Solve(input);
				if (output(0, 0) > 0.5f)
				{
					Flap(birds[i]);
				}
			}
		}

		for (Pipe& pipe : pipes)
		{
			pipe.position -= scaledDeltaTime * PIPE_SCROLL_SPEED;
			if (pipe.position < -32.f - PIPE_WIDTH)
			{
				pipe.position = 32.f - (std::abs(pipe.position - (-32.f - PIPE_WIDTH)));
				pipe.topHeight = Random::Range(2.f, 36.f - 2.f - PIPE_OPENING);
				pipe.scored = false;
			}

			if (!pipe.scored && pipe.position + PIPE_WIDTH < BIRD_X)
			{
				score++;
				if (score > highscore)
				{
					highscore = score;
					highscoreGeneration = generation;
				}

				pipe.scored = true;
			}

			// upper
			if (Physics::Box* box = physicsWorld.GetBoxByID(pipe.upperPhysicsID))
			{
				box->position = pipe.GetUpperRectPos();
				box->height = pipe.GetUpperRectHeight();
			}
			// lower
			if (Physics::Box* box = physicsWorld.GetBoxByID(pipe.lowerPhysicsID))
			{
				Vec2f pos(pipe.position, -18.f);
				float height = 36.f - pipe.topHeight - PIPE_OPENING;
				box->position = pos;
				box->height = height;
			}
		}

		for (int i = 0; i < birds.size(); i++)
		{
			Bird& bird = birds[i];
			if (!bird.alive)
				continue;

			bird.velocity -= scaledDeltaTime * GRAVITY;
			bird.position += bird.velocity * scaledDeltaTime;

			if (bird.position < -18.f)
			{
				bird.position = -18.f; // DEATH
				bird.velocity = 0.f;
				Kill(i);
			}
			if (bird.position > 18.f - BIRD_SIZE)
			{
				bird.position = 18.f - BIRD_SIZE;
				bird.velocity = 0.f;
				Kill(i);
			}
			if (Physics::Box* box = physicsWorld.GetBoxByID(bird.physicsID))
			{
				box->position = bird.GetRectPos();
			}
		}

		bool gameOver = true;
		for (Bird& bird : birds)
		{
			if (bird.alive)
			{
				gameOver = false;
				break;
			}
		}
		if (gameOver)
			GameOver();

		if (DISABLE_TRAINING_MODE_SCORE > 0 && score >= DISABLE_TRAINING_MODE_SCORE)
			isTrainingModeEnabled = false;

		physicsWorld.CheckCollisions();
		
		psw.Stop();
		timePassed = psw.GetMilli();

		updates++;
	} 
	while (isTrainingModeEnabled && isBotModeEnabled && timePassed < 1000.f/200.f);
}

void FlappyBot::Render()
{
	for (const Pipe& pipe : pipes)
	{
		if (!isPlaying)
			break;

		static Color pipeColor(67, 176, 71);
		// upper pipe
		{
			viewport.DrawRectangleSolid(pipe.GetUpperRectPos(), PIPE_WIDTH, pipe.GetUpperRectHeight(), pipeColor);
		}
		// lower pipe
		{
			viewport.DrawRectangleSolid(pipe.GetLowerRectPos(), PIPE_WIDTH, pipe.GetLowerRectHeight(), pipeColor);
		}
	}

	int firstAlive = -1;

	for (int i = 0; i < birds.size(); i++)
	{
		if (birds[i].alive)
		{
			firstAlive = i;
			break;
		}
	}

	for (Bird& bird : birds)
	{
		static Color botColor(229, 37, 33);
		static Color playerColor(0, 150, 255);

		if (bird.alive)
			viewport.DrawRectangleSolid(bird.GetRectPos(), BIRD_SIZE, BIRD_SIZE, isBotModeEnabled ? botColor : playerColor);
	}

	{
		int aliveCount = 0;
		for (const Bird& b : birds)
			if (b.alive)
				aliveCount++;

		std::ostringstream os;
		os << "\nGeneration: " << generation;
		os << "\nScore: " << score;
		if (isBotModeEnabled)
			os << "\nFitness: " << globalFitness;
		os << "\nHighscore: " << highscore;
		if (isBotModeEnabled)
			os << "(Gen " << highscoreGeneration << ")";
		os << "\nAlive: " << aliveCount << " / " << POPULATION;
		if (isBotModeEnabled)
			os << "\nTrainingMode: " << (isTrainingModeEnabled ? "ENABLED" : "DISABLED");
		
		if (updatesPerSecondTimer < 0.f)
		{
			updatesPerSecond = updates / 0.5f;
			updatesPerSecondTimer = 0.5f;
			updates = 0;
		}
		updatesPerSecondTimer -= GetDeltaTime();

		os << "\nSim frames per second: " << updatesPerSecond;
		os << "\nBest: " << GetBest(fitnesses);

		os << "\n\nKeys:\nB: toggle botmode\nR: reset";
		
		if (!isPlaying)
		{
			os << "\nSpace: Start";
		}
		else
		{
			if (!isBotModeEnabled)
			{
				os << "\nSpace: Flap";
			}
			else
			{
				os << "\nSpace: toggle training mode";
			}
		}

		//if (firstAlive > -1)
		//{
		//	Matrix input = GenNNInput(birds[firstAlive]);
		//	os << "\nInputs: " << input;
		//}

		DrawText(os.str(), 0, 0);
	}

	{
		int width = 500;
		int height = 300;
		Vec2i pos(GetWindowWidth() - width - 300 - 5, GetWindowHeight() - height);


		if (firstAlive > -1)
		{
			Matrix input = GenNNInput(birds[firstAlive]);

			nnDrawer.DrawNeuralNetwork(bots[firstAlive], &input ? &input : nullptr, pos, width, height, false, 10);
		}
	}

	{
		int size = 300;
		Vec2i pos(GetWindowWidth() - size, GetWindowHeight() - size);

		histogram.Draw(*this, pos, size, nullptr, true);
	}
}

void FlappyBot::HandleCollision(Physics::CollisionInfo col)
{
	if (!isPlaying)
		return;

	const Physics::Info& info2 = physicsWorld.GetInfoByID(col.ObjectID2);
	if (info2.layer != Physics::Layer::Environment)
		return;
	const Physics::Info& info1 = physicsWorld.GetInfoByID(col.ObjectID1);
	if (info1.layer != Physics::Layer::Agent)
		return;

	int birdIndex = -1;
	for (int i = 0; i < birds.size(); i++)
	{
		if (birds[i].physicsID == col.ObjectID1)
		{
			birdIndex = i;
			break;
		}
	}

	Kill(birdIndex);
}

void FlappyBot::Flap(Bird& bird)
{
	bird.velocity = BIRD_FLAP;
}

void FlappyBot::Kill(int birdIndex)
{
	if (!birds[birdIndex].alive)
		return;

	birds[birdIndex].alive = false;
	physicsWorld.GetInfoByID(birds[birdIndex].physicsID).enabled = false;

	if (isBotModeEnabled)
		fitnesses[birdIndex] = (float)globalFitness;
}

void FlappyBot::GameOver() 
{ 
	if (isBotModeEnabled)
	{
		histogram.RecordSample((float)globalFitness);
		IterateGeneticAlgorithm();
		ResetGame();
	}
	else
	{
		isPlaying = false;
	}
}

void FlappyBot::ResetGame()
{
	score = 0;
	globalFitness = 0.f;

	{
		pipes[0].position = 16.f;
		pipes[0].topHeight = Random::Range(2.f, 36.f - 2.f - PIPE_OPENING);


		pipes[1].position = pipes[0].position + 32.f + PIPE_WIDTH / 2.f;
		pipes[1].topHeight = Random::Range(2.f, 36.f - 2.f - PIPE_OPENING);

		for (const Pipe& pipe : pipes)
		{
			// upper
			Physics::Box* upperBox = physicsWorld.GetBoxByID(pipe.upperPhysicsID);
			upperBox->position = pipe.GetUpperRectPos();
			upperBox->height = pipe.GetUpperRectHeight();
			// lower
			Physics::Box* lowerBox = physicsWorld.GetBoxByID(pipe.lowerPhysicsID);
			Vec2f pos(pipe.position, -18.f);
			float height = 36.f - pipe.topHeight - PIPE_OPENING;
			lowerBox->position = pos;
			lowerBox->height = height;
		}
	}

	for (int i = 0; i < POPULATION; i++)
	{
		birds[i].position = -BIRD_SIZE / 2.f;
		birds[i].velocity = 0.f;
		Physics::Box* box = physicsWorld.GetBoxByID(birds[i].physicsID);
		box->position = Vec2f(BIRD_X, -BIRD_SIZE / 2.f);

		bool isAlive = isBotModeEnabled ? true : i == 0;
		birds[i].alive = isAlive;

		physicsWorld.GetInfoByID(birds[i].physicsID).enabled = isAlive;
	}
}

void FlappyBot::ResetBots()
{
	for (int i = 0; i < POPULATION; i++)
	{
		std::vector<unsigned> layerShapes;
		RandomizeLayerShapes(layerShapes);
		bots[i].SetLayerShapes(layerShapes);
	}
	generation = 0;
	highscore = 0;
	highscoreGeneration = 0;
	histogram.Reset();
}

const Pipe& FlappyBot::GetClosestPipe() const
{
	unsigned closestPipe = 0;
	float closest = 100.f;
	for (int j = 0; j < pipes.size(); j++)
	{
		if (pipes[j].position + PIPE_WIDTH > BIRD_X)
		{
			if (pipes[j].position + PIPE_WIDTH < closest)
			{
				closest = pipes[j].position + PIPE_WIDTH;
				closestPipe = j;
			}
		}
	}
	return pipes[closestPipe];
}

void FlappyBot::IterateGeneticAlgorithm()
{
	std::array<NeuralNetwork, POPULATION> newBots;
	/*for (NeuralNetwork& nn : newBots)
	{
		std::vector<unsigned> layerShapes;
		RandomizeLayerShapes(layerShapes);
		nn = NeuralNetwork(0.1f, layerShapes);
	}*/

	int count = 0;

	// Always retain best specimen from the previous generation
	{
		int parent1 = GetBest(fitnesses);
		NeuralNetwork& child = newBots[count];
		child = bots[parent1];

		count++;
	}

	int attempts = 0;

	while (count < POPULATION)
	{
		if (attempts > POPULATION * 10)
		{
			std::cout << "ERROR: IterateGeneticAlgorithm loop never finished" << std::endl;
			ResetBots();
			ResetGame();
		}
		attempts++;
		//	Selection
		int parent1 = GetWeightedRandom(fitnesses);
		//int parent2 = GetWeightedRandom(fitnesses);

		//if (parent1 == parent2)
		//	continue;

		NeuralNetwork& child = newBots[count];

		//	Crossover
 		//CrossOver(bots[parent1], bots[parent2], child);
		child = bots[parent1];

		//	Mutation
		Mutate(child);

		count++;
	}

	bots = newBots;
	generation++;
}

Matrix FlappyBot::GenNNInput(const Bird& bird) const
{
	const Pipe& closestsPipe = GetClosestPipe();
	float velInput = birds[0].velocity < 0.f ? 0.5f - (birds[0].velocity / -60.f) * 0.5f : 0.5f + (birds[0].velocity / BIRD_FLAP) * 0.5f;

	Matrix input(1, 5);
	input(0, 0) = (bird.position + 18.f) / 36.f;
	input(0, 1) = velInput;
	input(0, 2) = (closestsPipe.position + 32.f) / 64.f;
	input(0, 3) = 1.f - closestsPipe.topHeight / 36.f;
	input(0, 4) = 1.f - (closestsPipe.topHeight + PIPE_OPENING) / 36.f;

	return input;
}
