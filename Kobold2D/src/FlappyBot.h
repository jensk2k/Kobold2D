#pragma once
#include "GameState.h"
#include "Physics.h"
#include "NeuralNetwork.h"
#include "Histogram.h"
#include "NeuralNetworkDrawer.h"
#include <array>

#define PIPE_OPENING 12.f
#define PIPE_WIDTH 6.f
#define PIPE_SCROLL_SPEED 20.f
#define BIRD_SIZE 3.f
#define BIRD_X -24.f
#define BIRD_FLAP 46.f
#define GRAVITY 140.f

#define POPULATION 100
#define MUTATION_RATE .2f
#define DISABLE_TRAINING_MODE_SCORE 0

struct Pipe
{
    float position; // x
    float topHeight;
    unsigned upperPhysicsID;
    unsigned lowerPhysicsID;
    bool scored = false;
    Vec2f GetUpperRectPos() const { return Vec2f(position, 18.f - topHeight); }
    float GetUpperRectHeight() const { return topHeight; }
    Vec2f GetLowerRectPos() const { return Vec2f(position, -18.f); }
    float GetLowerRectHeight() const { return 36.f - topHeight - PIPE_OPENING; }
};

struct Bird
{
    float position; // y
    float velocity;
    unsigned physicsID;
    bool alive = true;
    Vec2f GetRectPos() const { return Vec2f(BIRD_X, position); }
};

class FlappyBot :
    public GameState
{
public:
    FlappyBot(Core& core) : 
        GameState(core),
        physicsWorld(0.1f, 100.f, [this](Physics::CollisionInfo col) { HandleCollision(col); }),
        viewport(*this, GetWindowHeight() / 36.f),
        nnDrawer(*this)
    {
        fitnesses.fill(0.f);
    }

    void Init() override;
    void HandleKeyDown(Keys key) override;
    void Update() override;
    void Render() override;

private:
    void HandleCollision(Physics::CollisionInfo col);
    void Flap(Bird& bird);
    void Kill(int birdIndex);
    void GameOver();
    void ResetGame();
    void ResetBots();
    const Pipe& GetClosestPipe() const;
    void IterateGeneticAlgorithm();
    Matrix GenNNInput(const Bird& bird) const;
    Physics::World physicsWorld;
    Viewport viewport;
    std::array<Pipe, 2> pipes;
    std::array<Bird, POPULATION> birds;
    std::array<NeuralNetwork, POPULATION> bots;
    std::array<float, POPULATION> fitnesses;

    bool isPlaying = false;
    bool isBotModeEnabled = true;
    bool isTrainingModeEnabled = false;
    float score = 0;
    int highscore = 0;
    int highscoreGeneration = 0;
    float globalFitness = 0.f;
    int generation = 1;
    //float timescale = 1.f;
    //int bestBot = -1;
    int updates = 0;
    int updatesPerSecond = 0;
    float updatesPerSecondTimer = 0.f;

    Histogram histogram;
    NeuralNetworkDrawer nnDrawer;
};

