#include "Cells.h"
#include "Random.h"
#include "MathUtils.h"
#include <sstream>

Cells::Cells(Core& core)
	: GameState(core)
	, trailMap(*this, 160, 90)
{
}

void Cells::Update()
{
	bool tick = false;
	if (tickTimer <= 0.f)
	{
		if (food.size() < 1000)
		{
			PropagateFood();
		}
		tick = true;
		tickTimer = 0.5f;
	}
	tickTimer -= GetDeltaTime();

	// consume energy
	if (tick)
	{
		for (int i = cells.size() - 1; i >= 0; i--)
		{
			Cell& cell = cells[i];
			float metabolism = cell.genes.size * 0.025f;
			float movementCost = cell.velocity.Length() * 0.01f * cell.genes.size;
			float foodDetectionCost = cell.genes.foodDetectionRange * 0.002f;

			cell.energy -= (metabolism + movementCost + foodDetectionCost);
			// Death
			if (cell.energy < 0.f)
			{
				//std::cout << "DEATH" << std::endl;
				cells.erase(cells.begin() + i);
			}
		}
	}

	// Extinction
	if (cells.size() == 0)
	{
		std::cout << "EXTINCTION after " << (int)simTime << " seconds" << std::endl;
		Reset();
	}

	for (int i = 0; i < cells.size(); i++)
	{
		Cell& cell = cells[i];

		Vec2f nearestFood;
		int nearestFoodIndex = -1;
		{
			int distance2 = Vec2f(GetWindowWidth(), GetWindowHeight()).Length2();

			for (int j = 0; j < food.size(); j++)
			{
				const Vec2f& foodPos = food[j];
				float cellToFoodDist2 = (cell.position - foodPos).Length2();

				if (cellToFoodDist2 < distance2)
				{
					distance2 = cellToFoodDist2;
					nearestFood = foodPos;
					nearestFoodIndex = j;
				}
			}

			// EAT
			if (nearestFoodIndex > -1 && distance2 < cell.genes.size * cell.genes.size)
			{
				food.erase(food.begin() + nearestFoodIndex);
				cell.energy += 1.f;
				cell.energy = MathUtils::Clamp(cell.energy, 0.f, cell.genes.MaxEnergy());
			}
		}

		int nearestOtherCellIndex = -1;
		{
			float distance2 = Vec2f(GetWindowWidth(), GetWindowHeight()).Length2();

			for (int j = 0; j < cells.size(); j++)
			{
				if (i == j)
					continue;

				Cell& otherCell = cells[j];
				const Vec2f& otherCellPos = otherCell.position;
				float distanceToOther2 = (cell.position - otherCellPos).Length2();

				if (distanceToOther2 < distance2)
				{
					distance2 = distanceToOther2;
					nearestOtherCellIndex = j;
				}
			}
		}
		
		Vec2f steeringDirection;
		bool satiated = (cell.energy / cell.genes.MaxEnergy()) > cell.genes.satiation;

		if (nearestFoodIndex > -1 && !satiated)
		{
			Vec2f foodVector = nearestFood - cell.position;
			if (foodVector.Length2() < MathUtils::Square(cell.genes.foodDetectionRange))
			{
				Vec2f foodDirection = foodVector.Normalized();
				steeringDirection = foodDirection;
			}
		}
		cell.repDirection = Vec2f();
		if (nearestOtherCellIndex > -1)
		{
			Vec2f repulsionVector = cell.position - cells[nearestOtherCellIndex].position;
			if (repulsionVector.Length2() < MathUtils::Square(cell.genes.repulsionRange))
			{
				Vec2f repulsionDirection = repulsionVector.Normalized();
				steeringDirection = repulsionDirection;
				cell.repDirection = repulsionDirection;
			}
		}

		bool shouldBreak = steeringDirection.IsZero();

		CalcSteering(steeringDirection, shouldBreak, cell.genes.maxSpeed, 10.f, 20.f, cell.position, cell.velocity);

		// Propagation
		if ((cell.energy / cell.genes.MaxEnergy()) > cell.genes.propagationMinEnergy && cell.propagationTimer > cell.genes.propagationInterval)
		{
			cell.energy /= 2.f;
			PropagateCell(&cell);

			cell.propagationTimer = 0.f;
		}
		cell.propagationTimer += GetDeltaTime();
	}

	if (histogramSampleTimer < 0.f)
	{
		histogramSampleTimer = 5.0f;

		Genes averageGenes;

		averageGenes.size = 0.f;
		averageGenes.maxSpeed = 0.f;
		averageGenes.foodDetectionRange = 0.f;
		averageGenes.repulsionRange = 0.f;
		averageGenes.propagationInterval = 0.f;
		averageGenes.propagationMinEnergy = 0.f;
		averageGenes.satiation = 0.f;
		averageGenes.mutationProbability = 0.f;

		// Histogram
		for (int i = 0; i < cells.size(); i++)
		{
			Cell& cell = cells[i];
			Genes& genes = cell.genes;

			averageGenes.size += genes.size;
			averageGenes.maxSpeed += genes.maxSpeed;
			averageGenes.foodDetectionRange += genes.foodDetectionRange;
			averageGenes.repulsionRange += genes.repulsionRange;
			averageGenes.propagationInterval += genes.propagationInterval;
			averageGenes.propagationMinEnergy += genes.propagationMinEnergy;
			averageGenes.satiation += genes.satiation;
			averageGenes.mutationProbability += genes.mutationProbability;
		}

		averageGenes.size /= cells.size();
		averageGenes.maxSpeed /= cells.size();
		averageGenes.foodDetectionRange /= cells.size();
		averageGenes.repulsionRange /= cells.size();
		averageGenes.propagationInterval /= cells.size();
		averageGenes.propagationMinEnergy /= cells.size();
		averageGenes.satiation /= cells.size();
		averageGenes.mutationProbability /= cells.size();

		histograms["population"].RecordSample((float)cells.size());
		histograms["size"].RecordSample(averageGenes.size);
		histograms["speed"].RecordSample(averageGenes.maxSpeed);
		histograms["food det"].RecordSample(averageGenes.foodDetectionRange);
		histograms["repulsion"].RecordSample(averageGenes.repulsionRange);
		histograms["prop int"].RecordSample(averageGenes.propagationInterval);
		histograms["prop min e"].RecordSample(averageGenes.propagationMinEnergy);
		histograms["satiation"].RecordSample(averageGenes.satiation);
		histograms["mut prob"].RecordSample(averageGenes.mutationProbability);

	}
	histogramSampleTimer -= GetDeltaTime();

	simTime += GetDeltaTime();

}

void Cells::Render()
{
	trailMap.Update(10.f);

	for (int i = 0; i < cells.size(); i++)
	{
		const Cell& cell = cells[i];
		int sizeSS = cell.genes.size * viewport.scale; 
		Vec2i cellPosSS = viewport.WorldToScreenSpace(cell.position);
		trailMap.Trail(cellPosSS, sizeSS, cell.genes.color);
	}
	trailMap.Render();

	{
		std::ostringstream os;
		os << "Simulation Time: " << (int)simTime;
		DrawText(os.str(), 0, 24);
	}

	{
		int highestGen = GetLastestGen();
		std::ostringstream os;
		os << "Generation: " << highestGen;
		DrawText(os.str(), 0, 48);
	}

	for (int i = 0; i < cells.size(); i++)
	{
		const Cell& cell = cells[i];
		int sizeSS = cell.genes.size * viewport.scale;

		Vec2i cellPosSS = viewport.WorldToScreenSpace(cell.position);

		//DrawCircleSolid(Vec2i(cellPosSS.x, cellPosSS.y), sizeSS, cell.genes.color);

		//float energy = cell.energy / cell.genes.MaxEnergy();
		////DrawRectangleSolid(cellPosSS + Vec2i(-8, -24), 16+2, 4+2, Colors::BLACK);
		//DrawRectangle(cellPosSS + Vec2i(-8, -24), 16 + 2, 4 + 2, Colors::WHITE);
		//DrawRectangleSolid(cellPosSS + Vec2i(-7, -23), 16 * energy, 4, Colors::RED);

		//DrawCircle(cellPosSS, cell.genes.foodDetectionRange * viewport.scale, Colors::WHITE);
		//DrawCircle(cellPosSS, cell.genes.repulsionRange * viewport.scale, Colors::RED);
		//DrawArrowWS(cell.position, cell.repDirection, 1.f, Colors::WHITE);
	}

	for (int i = 0; i < food.size(); i++)
	{
		const Vec2f& foodPos = food[i];
		int size = 4;

		Vec2i foodPosSS = viewport.WorldToScreenSpace(Vec2f(foodPos.x, foodPos.y));

		DrawRectangleSolid(Vec2i(foodPosSS.x - size / 2, foodPosSS.y - size / 2), size, size, Colors::WHITE);
	}

	int edgeOffset = 20;
	int histogramSize = 120;

	int i = 0;
	for (auto const& x : histograms)
	{
		Vec2i histPos(edgeOffset + (histogramSize + edgeOffset) * i, GetWindowHeight() - edgeOffset - histogramSize);
		
		const std::string& title = x.first;
		const Histogram& hist = x.second;

		hist.Draw(*this, histPos, histogramSize, title);

		i++;
	}
}

void Cells::PropagateCell(const Cell* parent)
{
	//std::cout << "PROPAGATION";
	Vec2f pos;
	if (parent)
	{
		float offset = parent->genes.size / 2.f;
		pos = parent->position + Vec2f(Random::Range(-offset, offset), Random::Range(-offset, offset));
	}
	else
	{
		pos = RandomScreenPosition();
	}
	
	Genes newGenes;
	if (parent)
	{
		newGenes = parent->genes;
		newGenes.generation++;

		// MUTATE
		if (Random::Range(0.f, 1.f) < parent->genes.mutationProbability)
		{
			//newGenes = parent->genes;
			int colorMutateRange = 16;
			newGenes.color.red = parent->genes.color.red + Random::Range(-colorMutateRange, colorMutateRange);
			newGenes.color.green = parent->genes.color.green + Random::Range(-colorMutateRange, colorMutateRange);
			newGenes.color.blue = parent->genes.color.blue + Random::Range(-colorMutateRange, colorMutateRange);


			std::cout << " MUTATION" << std::endl;
			std::cout << " color: " << parent->genes.color << " -> " << newGenes.color << std::endl;

			int dice = Random::Range(0, 8);
			switch (dice)
			{
			case 0: // size
			{
				float sizeMutateRange = parent->genes.size * 0.2f;
				newGenes.size = MathUtils::Max(0.1f, parent->genes.size + Random::Range(-sizeMutateRange, sizeMutateRange));
				std::cout << " size: " << parent->genes.size << " -> " << newGenes.size << std::endl;
				break;
			}
			case 1: // speed
			{
				float maxSpeedMutateRange = parent->genes.maxSpeed * 0.2f;
				newGenes.maxSpeed = MathUtils::Max(0.f, parent->genes.maxSpeed + Random::Range(-maxSpeedMutateRange, maxSpeedMutateRange));
				std::cout << " maxSpeed: " << parent->genes.maxSpeed << " -> " << newGenes.maxSpeed << std::endl;
				break;
			}
			case 2: // food detection
			{
				float foodDetectionMutateRange = parent->genes.foodDetectionRange * 0.2f;
				newGenes.foodDetectionRange = MathUtils::Max(0.f, parent->genes.foodDetectionRange + Random::Range(-foodDetectionMutateRange, foodDetectionMutateRange));
				std::cout << " foodDetectionRange: " << parent->genes.foodDetectionRange << " -> " << newGenes.foodDetectionRange << std::endl;
				break;
			}
			case 3: // repulsion
			{
				float repulsionMutateRange = parent->genes.repulsionRange * 0.2f;
				newGenes.repulsionRange = MathUtils::Max(newGenes.size, parent->genes.repulsionRange + Random::Range(-repulsionMutateRange, repulsionMutateRange));
				std::cout << " repulsionRange: " << parent->genes.repulsionRange << " -> " << newGenes.repulsionRange;
				break;
			}
			case 4: // prop interval
			{
				float propagationIntervalMutateRange = parent->genes.propagationInterval * 0.2f;
				newGenes.propagationInterval = MathUtils::Max(0.1f, parent->genes.propagationInterval + Random::Range(-propagationIntervalMutateRange, propagationIntervalMutateRange));
				std::cout << " propagationInterval: " << parent->genes.propagationInterval << " -> " << newGenes.propagationInterval << std::endl;
				break;
			}
			case 5: // prop min energy
			{
				newGenes.propagationMinEnergy = MathUtils::Clamp(parent->genes.propagationMinEnergy + Random::Range(-0.1f, 0.1f), 0.1f, 1.f);
				std::cout << " propagationMinEnergy: " << parent->genes.propagationMinEnergy << " -> " << newGenes.propagationMinEnergy << std::endl;
				break;
			}
			case 6:
			{
				newGenes.satiation = MathUtils::Clamp(parent->genes.satiation + Random::Range(-0.1f, 0.1f), 0.1f, 1.f);
				std::cout << " satation: " << parent->genes.satiation << " -> " << newGenes.satiation << std::endl;
				break;
			}
			case 7:
			{
				newGenes.mutationProbability = MathUtils::Clamp(parent->genes.mutationProbability + Random::Range(-0.1f, 0.1f), 0.f, 1.f);
				std::cout << " mutationProbability: " << parent->genes.mutationProbability << " -> " << newGenes.mutationProbability << std::endl;
				break;
			}
			default:
				std::cout << " UNMAPPED GENE" << std::endl;
				break;
			}
		}
	}
	else
	{
		newGenes.color.red = Random::Range(0, 255);
		newGenes.color.green = Random::Range(0, 255);
		newGenes.color.blue = Random::Range(0, 255);
		newGenes.size = Random::Range(0.1f, 2.f);
		newGenes.maxSpeed = Random::Range(1.f, 10.f);
		newGenes.foodDetectionRange = Random::Range(5.f, 20.f);
		newGenes.repulsionRange = MathUtils::Max(newGenes.size, Random::Range(1.f, 10.f));
		newGenes.propagationInterval = Random::Range(1.f, 10.f);
		newGenes.propagationMinEnergy = Random::Range(0.f, 1.f);
		newGenes.satiation = MathUtils::Clamp(newGenes.propagationMinEnergy + Random::Range(0.0f, 1.f), 0.f, 1.f);
		newGenes.mutationProbability = 0.25f;
	}

	//std::cout << std::endl;

	float birthEnergy = parent ? parent->energy : newGenes.MaxEnergy() / 2.f;

	cells.push_back(Cell());
	Cell& newCell = cells[cells.size() - 1];
	newCell.position = pos;
	newCell.energy = birthEnergy;
	newCell.genes = newGenes;
}

Vec2f Cells::RandomScreenPosition()
{
	return viewport.ScreenToWorldPosition(Vec2i(Random::Range(0, GetWindowWidth()), Random::Range(0, GetWindowHeight())));
}

void Cells::HandleKeyDown(Keys key)
{
	if (key == Keys::R)
	{
		Reset();
	}
}

void Cells::CalcSteering(Vec2f steeringDirection, bool shouldBreak, float maxSpeed, float maxAcceleration, float maxDeceleration, Vec2f& position, Vec2f& velocity)
{
	Vec2f acceleration;
	float deltaTime = GetDeltaTime();
	
	if (!shouldBreak)
	{
		Vec2f lateral = Vec2f::Rotate(steeringDirection, PI / 2.f);

		float lateralVelocityScalarProj = Vec2f::Dot(lateral, velocity);
		float correction = lateralVelocityScalarProj / maxSpeed;

		acceleration = (steeringDirection * (1.f - correction) - lateral * correction) * maxAcceleration;

	}
	else if (velocity.Length2() > 0.f)
	{
		//break
		acceleration = velocity.Normalized() * -maxDeceleration;
	}

	velocity += acceleration * deltaTime;

	if (velocity.Length2() > maxSpeed * maxSpeed)
	{
		velocity = velocity.Normalized() * maxSpeed;
	}

	// s = s0 + v0*t + 0.5*a*t^2
	position += velocity * deltaTime;
}

void Cells::PropagateFood()
{
	Vec2f randomFood = food.size() > 1 ? food[Random::Range(0, food.size() - 1)] : Vec2f();

	float spread = 12.f;
	Vec2f propagatedFoodPosition = randomFood + Vec2f(Random::Range(-spread, spread), Random::Range(-spread, spread));
	Vec2f screenMin = viewport.ScreenToWorldPosition(Vec2i(0, 0));
	Vec2f screenMax = viewport.ScreenToWorldPosition(Vec2i(GetWindowWidth(), GetWindowHeight()));

	propagatedFoodPosition.x = MathUtils::Clamp(propagatedFoodPosition.x, screenMin.x, screenMax.x);
	propagatedFoodPosition.y = MathUtils::Clamp(propagatedFoodPosition.y, -screenMin.y, -screenMax.y);

	food.push_back(propagatedFoodPosition);
}

void Cells::SpeadFood(int)
{
	for (int i = 0; i < 100; i++)
	{
		Vec2f propagatedFoodPosition = RandomScreenPosition();

		food.push_back(propagatedFoodPosition);
	}
}

void Cells::Reset()
{
	histograms.clear();
	simTime = 0.f;

	food.clear();
	cells.clear();

	SpeadFood(100);
	PropagateCell(nullptr);
}

int Cells::GetLastestGen() const
{
	int highestGen = 0;
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells[i].genes.generation > highestGen)
			highestGen = cells[i].genes.generation;
	}

	return highestGen;
}

void Histogram::RecordSample(float sample)
{
	histogram.push_back(sample);
}

void Histogram::Draw(GameState& gameState, Vec2i position, int histogramSize, const std::string& title) const
{
	gameState.DrawRectangle(position, histogramSize, histogramSize, Colors::WHITE);

	{
		std::ostringstream ss;
		ss << title;
		Vec2i textPos = position + Vec2i(0, -30);

		gameState.DrawText(ss.str(), textPos.x, textPos.y);
	}

	if (histogram.size() > 1)
	{
		Vec2i bottomLeft(position.x, position.y + histogramSize);
		float highestSampleValue = 0.f;
		for (int i = 0; i < histogram.size(); i++)
		{
			float sample = histogram[i];
			if (sample > highestSampleValue)
				highestSampleValue = sample;
		}

		for (int i = 0; i < histogram.size() - 1; i++)
		{
			float sample1 = histogram[i];
			float x1 = bottomLeft.x + ((float)i / (float)(histogram.size() - 1)) * histogramSize;
			float y1 = bottomLeft.y - ((float)sample1 / (float)highestSampleValue) * histogramSize;

			float sample2 = histogram[i + 1];
			float x2 = bottomLeft.x + ((float)(i + 1) / (float)(histogram.size() - 1)) * histogramSize;
			float y2 = bottomLeft.y - ((float)sample2 / (float)highestSampleValue) * histogramSize;
			gameState.DrawLine(Vec2i(x1, y1), Vec2i(x2, y2), Colors::YELLOW);
		}


		{
			std::ostringstream ss;
			ss << ((int)(100.f * highestSampleValue)) / 100.f << " " << ((int)(100.f * histogram[histogram.size() - 1])) / 100.f;
			Vec2i textPos = position + Vec2i(0, histogramSize - 30);

			gameState.DrawText(ss.str(), textPos.x, textPos.y);
		}
	}
}
