#include "Cells.h"
#include "Random.h"
#include <sstream>
#include <sstream>

Cells::Cells(Core& core)
	: GameState(core)
	, trailMap(*this, 160, 90)
	, metabolismTimer(1.f)
	, food(160, 90)
{
	Reset();
}

void Cells::Update()
{
	for (int i = 0; i < cells.size(); i++)
	{
		Cell& cell = cells[i];
		cell.nearestPlant.exists = false;
		cell.nearestMeat.exists = false;
	}

	// handle food
	for (int y = 0; y < food.m_height; y++)
	{
		for (int x = 0; x < food.m_width; x++)
		{
			if (food(x, y).state == FoodState::Growing)
			{
				if (food(x, y).growthTimer.Update(GetDeltaTime()))
				{
					food(x, y).state = FoodState::Plant;
				}
			}
			else
			{
				int size = GetWindowWidth() / food.m_width;
				Vec2i ssPos(x * size + size/2, y * size + size / 2);
				Vec2f wsPos = viewport.ScreenToWorldPosition(ssPos);

				for (int i = 0; i < cells.size(); i++)
				{
					Cell& cell = cells[i];
					float cellToFoodDist2 = (cell.position - wsPos).Length2();
					
					// detect nearest food
					if (food(x, y).state == FoodState::Plant)
					{
						if (!cell.nearestPlant.exists || cellToFoodDist2 < (cell.position - cell.nearestPlant.position).Length2())
						{
							cell.nearestPlant.exists = true;
							cell.nearestPlant.position = wsPos;
						}
					}
					else if (food(x, y).state == FoodState::Meat)
					{
						if (!cell.nearestMeat.exists || cellToFoodDist2 < (cell.position - cell.nearestMeat.position).Length2())
						{
							cell.nearestMeat.exists = true;
							cell.nearestMeat.position = wsPos;
						}
					}

					// Eat
					bool satiated = (cell.energy / cell.MaxEnergy()) >= 1.f;
					if (!satiated && cellToFoodDist2 < MathUtils::Square(cell.Size()))
					{
						float foodEnergy = 0.f;
						if (food(x, y).state == FoodState::Plant)
							foodEnergy = (1.f - cell.genes.diet) * 1.f;
						else if (food(x, y).state == FoodState::Meat)
							foodEnergy = cell.genes.diet * 10.f;

						cell.energy = MathUtils::Clamp(cell.energy + foodEnergy, 0.f, cell.MaxEnergy());
						food(x, y).state = FoodState::Growing;
					}
				}
			}
		}
	}

	// consume energy and age
	if (metabolismTimer.Update(GetDeltaTime()))
	{
		for (int i = cells.size() - 1; i >= 0; i--)
		{
			Cell& cell = cells[i];
			float sizeCost = cell.Size() * 0.1f;
			float movementCost = cell.velocity.Length() * 0.01f * cell.Size();
			//float foodDetectionCost = cell.genes.foodDetectionRange * 0.002f;

			cell.energy -= (0.1f + sizeCost + movementCost);

			bool kill = cell.energy <= 0.f || cell.age > cell.genes.lifespan;

			// Death
			if (kill)
			{
				Vec2i foodCoord = WorldToFoodCoord(cell.position);
				food(foodCoord.x, foodCoord.y).state = FoodState::Meat;
				food(foodCoord.x, foodCoord.y).growthTimer.timer = 0.f;
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

		cell.age += GetDeltaTime();

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
		
		cell.steeringDirection = Vec2f();
		bool satiated = (cell.energy / cell.MaxEnergy()) >= 1.f;
		if (!satiated)
		{
			float plantEnergy = cell.nearestPlant.exists ? (1.f - cell.genes.diet) * 1.f : 0.f;
			float meatEnergy = cell.nearestMeat.exists ? cell.genes.diet * 10.f : 0.f;

			if (cell.nearestPlant.exists && plantEnergy > meatEnergy)
			{
				Vec2f foodVector = cell.nearestPlant.position - cell.position;
				if (foodVector.Length2() < MathUtils::Square(cell.genes.detectionRange)
					&& foodVector.Length2() > MathUtils::Square(cell.Size()))
				{
					Vec2f foodDirection = foodVector.Normalized();
					cell.steeringDirection = foodDirection;
				}
			}
			if (cell.nearestMeat.exists && plantEnergy < meatEnergy)
			{
				Vec2f foodVector = cell.nearestMeat.position - cell.position;
				if (foodVector.Length2() < MathUtils::Square(cell.genes.detectionRange)
					&& foodVector.Length2() > MathUtils::Square(cell.Size()))
				{
					Vec2f foodDirection = foodVector.Normalized();
					cell.steeringDirection = foodDirection;
				}
			}

		}

		bool shouldBreak = cell.steeringDirection.IsZero();

		CalcSteering(cell.steeringDirection, shouldBreak, cell.genes.maxSpeed, 10.f, 20.f, cell.position, cell.velocity);

		// Propagation
		if (cell.age > cell.genes.matureAge
			&& cell.propagationTimer.Update(GetDeltaTime())
			&& (cell.energy / cell.MaxEnergy()) > cell.genes.propagationMinEnergy)
		{
			Genes parentGenesCopy = cell.genes;
			Vec2f parentPosition = cell.position;

			cell.energy -= cell.genes.startSize*10.f;
			PropagateCell(parentPosition, &parentGenesCopy, false);
		}


		//Vec2i foodCoord = WorldToFoodCoord(cell.position);
		//food(foodCoord.x, foodCoord.y).state = FoodState::Meat;
	}

	// Handle collisions
	for (int i = 0; i < cells.size(); i++)
	{
		Cell& cell1 = cells[i];
		for (int j = 0; j < cells.size(); j++)
		{
			if (j == i)
				continue;

			Cell& cell2 = cells[j];
			Vec2f vector = cell2.position - cell1.position;
			if (vector.Length2() < MathUtils::Square(cell1.Size() + cell2.Size()))
			{
				float distance = vector.Length() - cell1.Size() - cell2.Size();
				// Collision detected

				Vec2f normal = vector.Normalized();
				cell1.position += normal * distance / 2.f;
				cell2.position -= normal * distance / 2.f;
			}
		}
	}

	if (histogramSampleTimer < 0.f)
	{
		histogramSampleTimer = 5.0f;

		Genes averageGenes;

		// Histogram
		for (int i = 0; i < cells.size(); i++)
		{
			Cell& cell = cells[i];
			Genes& genes = cell.genes;

			averageGenes.matureSize += genes.matureSize;
			averageGenes.startSize += genes.startSize;
			averageGenes.maxSpeed += genes.maxSpeed;
			averageGenes.detectionRange += genes.detectionRange;
			averageGenes.propagationInterval += genes.propagationInterval;
			averageGenes.propagationMinEnergy += genes.propagationMinEnergy;
			averageGenes.mutationProbability += genes.mutationProbability;
			averageGenes.lifespan += genes.lifespan;
			averageGenes.matureAge += genes.matureAge;
			averageGenes.diet += genes.diet;
		}

		averageGenes.matureSize /= cells.size();
		averageGenes.startSize/= cells.size();
		averageGenes.maxSpeed /= cells.size();
		averageGenes.detectionRange /= cells.size();
		averageGenes.propagationInterval /= cells.size();
		averageGenes.propagationMinEnergy /= cells.size();
		averageGenes.mutationProbability /= cells.size();
		averageGenes.lifespan /= cells.size();
		averageGenes.matureAge /= cells.size();
		averageGenes.diet /= cells.size();

		histograms["population"].RecordSample((float)cells.size());
		histograms["mature size"].RecordSample(averageGenes.matureSize);
		histograms["start size"].RecordSample(averageGenes.startSize);
		histograms["max speed"].RecordSample(averageGenes.maxSpeed);
		histograms["detection"].RecordSample(averageGenes.detectionRange);
		histograms["prop int"].RecordSample(averageGenes.propagationInterval);
		histograms["prop min e"].RecordSample(averageGenes.propagationMinEnergy);
		histograms["mut prob"].RecordSample(averageGenes.mutationProbability);
		histograms["lifespan"].RecordSample(averageGenes.lifespan);
		histograms["mature age"].RecordSample(averageGenes.matureAge);
		histograms["diet"].RecordSample(averageGenes.diet);

	}
	histogramSampleTimer -= GetDeltaTime();

	simTime += GetDeltaTime();

}

#define TRAILS 1
void Cells::Render()
{
#if TRAILS
	trailMap.Update(1.f);

	for (int i = 0; i < cells.size(); i++)
	{
		const Cell& cell = cells[i];
		int sizeSS = cell.Size() * viewport.scale; 
		Vec2i cellPosSS = viewport.WorldToScreenSpace(cell.position);
		trailMap.Trail(cellPosSS, sizeSS, cell.genes.color);
		
	}
	trailMap.Render();
#endif

	for (int y = 0; y < food.m_height; y++)
	{
		for (int x = 0; x < food.m_width; x++)
		{
			int size = GetWindowWidth() / food.m_width;
			Vec2i ssPos(x * size, y * size);

			if (food(x, y).state == FoodState::Plant)
			{
				DrawRectangleSolid(ssPos, size, size, Colors::WHITE);
			}
			else if (food(x, y).state == FoodState::Meat)
			{
				DrawRectangleSolid(ssPos, size, size, Colors::RED);
			}
		}
	}

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

	//{
	//	std::ostringstream os;
	//	os << "Food: " << food.size();
	//	DrawText(os.str(), 0, 72);
	//}

#if !TRAILS
	for (int i = 0; i < cells.size(); i++)
	{
		const Cell& cell = cells[i];
		int sizeSS = cell.Size() * viewport.scale;

		Vec2i cellPosSS = viewport.WorldToScreenSpace(cell.position);
		
		//DrawCircle(Vec2i(cellPosSS.x, cellPosSS.y), sizeSS, Colors::WHITE);
		DrawCircleSolid(Vec2i(cellPosSS.x, cellPosSS.y), sizeSS, cell.genes.color);
		DrawCircle(Vec2i(cellPosSS.x, cellPosSS.y), sizeSS, Colors::WHITE);

		float energy = cell.energy / cell.MaxEnergy();
		DrawRectangleSolid(cellPosSS + Vec2i(-8, -24), 16+2, 4+2, Colors::BLACK);
		DrawRectangle(cellPosSS + Vec2i(-8, -24), 16 + 2, 4 + 2, Colors::WHITE);
		DrawRectangleSolid(cellPosSS + Vec2i(-7, -23), 16 * energy, 4, Colors::RED);

		//DrawCircle(cellPosSS, cell.genes.foodDetectionRange * viewport.scale, Colors::WHITE);
		//DrawCircle(cellPosSS, cell.genes.repulsionRange * viewport.scale, Colors::RED);
		DrawArrowWS(cell.position, cell.steeringDirection, 1.f, Colors::WHITE);
	}
#endif

	//for (int i = 0; i < food.size(); i++)
	//{
	//	const Vec2f& foodPos = food[i];
	//	int size = 4;

	//	Vec2i foodPosSS = viewport.WorldToScreenSpace(Vec2f(foodPos.x, foodPos.y));

	//	DrawRectangleSolid(Vec2i(foodPosSS.x - size / 2, foodPosSS.y - size / 2), size, size, Colors::WHITE);
	//}

	int edgeOffset = 10;
	int histogramSize = 115;

	int i = 0;
	for (auto const& x : histograms)
	{
		Vec2i histPos(edgeOffset + (histogramSize + edgeOffset) * i, GetWindowHeight() - edgeOffset - histogramSize);
		
		const std::string& title = x.first;
		const Histogram& hist = x.second;

		hist.Draw(*this, histPos, histogramSize, &title);

		i++;
	}
}

void Cells::PropagateCell(Vec2f spawnPosition, const Genes* parentGenes, bool disableMutation)
{
	//std::cout << "PROPAGATION" << std::endl;
	Vec2f pos;
	if (parentGenes)
	{
		pos = spawnPosition + Vec2f(Random::Range(-1.f, 1.f), Random::Range(-1.f, 1.f)).Normalized();// *parentGenes->maxSize * 2.f;
	}
	else
	{
		pos = RandomScreenPosition();
	}
	
	Genes newGenes;
	if (parentGenes)
	{
		newGenes = *parentGenes;
		newGenes.generation++;

		// MUTATE
		if (Random::Range(0.f, 1.f) < parentGenes->mutationProbability)
		{
			//newGenes = parent->genes;
			int colorMutateRange = 16;
			newGenes.color.red = parentGenes->color.red + Random::Range(-colorMutateRange, colorMutateRange);
			newGenes.color.green = parentGenes->color.green + Random::Range(-colorMutateRange, colorMutateRange);
			newGenes.color.blue = parentGenes->color.blue + Random::Range(-colorMutateRange, colorMutateRange);


			std::cout << " MUTATION" << std::endl;
			std::cout << " color: " << parentGenes->color << " -> " << newGenes.color << std::endl;

			int dice = Random::Range(0, 10);
			switch (dice)
			{
			case 0: // size
			{
				newGenes.startSize = MutateGene(parentGenes->startSize, "startSize", 0.1f);
				break;
			}
			case 1: 
			{
				newGenes.matureSize = MutateGene(parentGenes->matureSize, "matureSize", parentGenes->startSize);
				break;
			}
			case 2: // speed
			{
				newGenes.maxSpeed = MutateGene(parentGenes->maxSpeed, "maxSpeed", 0.f);
				break;
			}
			case 3: // detection
			{
				newGenes.detectionRange = MutateGene(parentGenes->detectionRange, "detectionRange", 0.f);
				break;
			}
			case 4: // propagation interval
			{
				newGenes.propagationInterval = MutateGene(parentGenes->propagationInterval, "propagationInterval", 0.1f);
				break;
			}
			case 5: // prop min energy
			{ 
				newGenes.propagationMinEnergy = MutateGene(parentGenes->propagationMinEnergy, "propagationMinEnergy", 0.1f, 1.f);
				break;
			}
			case 6: // mut prop
			{
				newGenes.mutationProbability = MutateGene(parentGenes->mutationProbability, "mutationProbability", 0.f, 1.f);
				break;
			}
			case 7: // life span
			{
				newGenes.lifespan = MutateGene(parentGenes->lifespan, "lifespan", 0.f);
				break;
			}
			case 8: // mature age
			{
				newGenes.matureAge = MutateGene(parentGenes->matureAge, "matureAge", 0.f);
				break;
			}
			case 9:
			{
				newGenes.diet = MutateGene(parentGenes->diet, "diet", 0.f, 1.f);
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
		newGenes.matureSize = (Random::Range(0.25f, 1.f)* Random::Range(0.25f, 1.f)) * 5.f;
		newGenes.startSize = Random::Range(0.25f, 1.f);
		newGenes.maxSpeed = Random::Range(1.f, 20.f);
		newGenes.detectionRange = Random::Range(0.f, 50.f);
		newGenes.propagationInterval = Random::Range(0.f, 60.f);
		newGenes.propagationMinEnergy = Random::Range(0.f, 1.f);
		newGenes.mutationProbability = 0.25f;
		newGenes.lifespan = Random::Range(0.f, 60.f * 10.f);
		newGenes.matureAge = Random::Range(0.f, 60.f * 1.f);
		newGenes.diet = Random::Range(0.f, 1.f);
	}

	//std::cout << std::endl;

	cells.push_back(Cell());
	Cell& newCell = cells[cells.size() - 1];
	newCell.position = pos;
	newCell.energy = newGenes.startSize * 10.f;
	newCell.genes = newGenes;
	newCell.propagationTimer.reset = newGenes.propagationInterval;
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
	
	Vec2f screenMin = viewport.ScreenToWorldPosition(Vec2i(0, 0));
	Vec2f screenMax = viewport.ScreenToWorldPosition(Vec2i(GetWindowWidth(), GetWindowHeight()));
	position.x = MathUtils::Clamp(position.x, screenMin.x, screenMax.x);
	position.y = MathUtils::Clamp(position.y, -screenMin.y, -screenMax.y);
}

float Cells::MutateGene(float parentGene, const std::string& name, float min, float max) const
{
	float mutationRange = parentGene * 0.2f;
	float mutatedGene = MathUtils::Max(min, parentGene + Random::Range(-mutationRange, mutationRange));
	if (max > -1.f)
		mutatedGene = MathUtils::Min(mutatedGene, max);
	std::cout << " " << name<< ": " << parentGene << " -> " << mutatedGene<< std::endl;
	return mutatedGene;
}

Vec2i Cells::WorldToFoodCoord(Vec2f worldPos) const
{
	int size = GetWindowWidth() / food.m_width;
	Vec2i ssPos = viewport.WorldToScreenSpace(worldPos);
	Vec2i foodPos = (ssPos/* - Vec2i(size / 2, size / 2)*/) / size;
	foodPos.x = MathUtils::Clamp(foodPos.x, 0, food.m_width - 1);
	foodPos.y = MathUtils::Clamp(foodPos.y, 0, food.m_height - 1);

	return foodPos;
}

void Cells::Reset()
{
	histograms.clear();
	simTime = 0.f;

	for (int y = 0; y < food.m_height; y++)
	{
		for (int x = 0; x < food.m_width; x++)
		{
			food(x, y) = Food();
			float reset = 60.f * 30.f;
			food(x, y).growthTimer = Timer(reset);
			food(x, y).growthTimer.timer = Random::Range(0.f, reset);
		}
	}

	cells.clear();
	
	//const Cell* parent = &cells[0];
	/*Genes parentGenesCopy = parent->genes;
	Vec2f parentPosition = cells[0].position;
	int nInitialChildren = Random::Range(0, 10);*/
	//int nInitialSpecies = Random::Range(0, 10);
	int nInitialSpecies = 1;
	for (int i = 0; i < nInitialSpecies; i++)
	{
		PropagateCell(RandomScreenPosition(), nullptr, true);
	}
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
