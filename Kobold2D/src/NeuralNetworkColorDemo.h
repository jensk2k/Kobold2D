#pragma once
#include "GameState.h"
#include "Matrix.h"
#include <vector>
#include <array>
#include <string>

struct Rect2i
{
    Rect2i(int x, int y, int width, int height)
        : pos(x, y),
        width(width),
        height(height)
    {}

    Vec2i pos;
    int width;
    int height;
};


struct NNetwork
{
    NNetwork() {};
    NNetwork(std::vector<int>& setup);
    //std::vector<float> inputLayer;
    std::vector<std::vector<float>> layers;
    //std::vector<float> outputLayer;

    std::vector<std::vector<float>> connectionsList;
    std::vector<std::vector<float>> biasesList;
};

struct NNGradient
{
    void Resize(const NNetwork& nn);
    std::vector<std::vector<float>> connectionsList;
    std::vector<std::vector<float>> biasesList;
};

class NeuralNetworkColorDemo :
    public GameState
{
public:
    NeuralNetworkColorDemo(Core& core);
    void Update() override;
    void Render() override;
    void HandleKeyDown(Keys key) override;
private:
    void RandomizeNN();
    void RandomizeInput();

    void DrawAllLayers();
    void DrawLayer(Rect2i rect, int layerIndex);
    void DrawConnections(Rect2i rect, int connectionsIndex);
    Vec2i GetNeuronDrawPosition(Rect2i rect, int layerIndex, int neuronIndex);
    int GetConnectionIndex(int layer1, int layer2, int neuron1, int neuron2);
    float GetConnectionWeight(int layer1, int layer2, int neuron1, int neuron2);
    float GetNeuronBias(int layer, int neuron);
    void SolveNN();
    void SolveLayerActivations(int layer);
    int GetNNAnswer();
    float CalcCost(int correct);
    void MassTest();
    void GetOutputLabel(std::string& label, int outputNeuron);
    int GetCorrectAnswer(Color color);
    int GetColorDiff(Color color1, Color color2);

    void BackProb();

    NNetwork nn;

    Color randomInputColor;

    int currentNNAnswer = 0;
    float currentCost = 0.f;

    int testIterations = 0;
    float accumulatedCost = 0.f;
    bool testing = false;
    int correctGuesses = 0;
};

