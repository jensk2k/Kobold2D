#include "NeuralNetwork.h"
#include "Random.h"

NeuralNetwork::NeuralNetwork(float learningRate, const std::vector<unsigned>& layerShapes)
	: learningRate(learningRate)
	, layerShapes(layerShapes)
{
	for (int i = 0; i < layerShapes.size() - 1; i++)
	{
		weights.push_back(Matrix(layerShapes[i], layerShapes[i + 1]));
		biases.push_back(Matrix(1, layerShapes[i + 1]));
	}
	RandomizeWeightsAndBiases();
}

void NeuralNetwork::RandomizeWeightsAndBiases()
{
	for (int i = 0; i < weights.size(); i++)
	{
		Matrix& weightsMat = weights[i];
		for (int row = 0; row < weightsMat.Rows(); row++)
		{
			for (int col = 0; col < weightsMat.Columns(); col++)
			{
				float rand = Random::Range(-1.f, 1.f) * Random::Range(-1.f, 1.f) * 4.f;

				weightsMat(col, row) = rand;
			}
		}
	}

	for (int i = 0; i < biases.size(); i++)
	{
		Matrix& biasesMat = biases[i];
		for (int row = 0; row < biasesMat.Rows(); row++)
		{
			float rand = Random::Range(-1.f, 1.f) * Random::Range(-1.f, 1.f) * 4.f;

			biasesMat(0, row) = rand;
		}
	}
}

Matrix NeuralNetwork::Solve(const Matrix& input) const
{
	Matrix activations = input;
	for (int i = 0; i < weights.size(); i++)
	{
		activations = Matrix::Add(Matrix::Product(weights[i], activations), biases[i]);

		// apply activation function
		for (int a = 0; a < activations.Rows(); a++)
		{
			activations(0, a) = Sigmoid(activations(0, a));
		}
	}
	return activations;
}

void NeuralNetwork::CalcActivations(const Matrix& inputs, std::vector<Matrix>& activations) const
{
	activations.resize(layerShapes.size());

	Matrix layer = inputs;
	activations[0] = layer;
	for (unsigned i = 0; i < weights.size(); i++)
	{
		layer = Matrix::Add(Matrix::Product(weights[i], layer), biases[i]);

		// apply activation function
		for (int a = 0; a < layer.Rows(); a++)
		{
			layer(0, a) = Sigmoid(layer(0, a));
		}

		activations[i + 1] = layer;
	}
}

void NeuralNetwork::BackPropagate(const Matrix& inputs, const Matrix& targets)
{
	std::vector<Matrix> activations;
	CalcActivations(inputs, activations);

	Matrix outputs = activations[activations.size() - 1];

	Matrix outputErrors = Matrix::Subtract(targets, outputs);

	Matrix errors = outputErrors;

	for (int i = weights.size() - 1; i >= 0; i--)
	{
		Matrix activationsRight = activations[i + 1];
		Matrix activationsLeft = activations[i];

		// do stuff
		Matrix gradients(activationsRight.Columns(), activationsRight.Rows());
		for (int j = 0; j < activationsRight.Rows(); j++)
		{
			gradients(0, j) = PsuedoDerivativeSigmoid(activationsRight(0, j));
		}
		gradients = Matrix::HadamardProduct(gradients, errors);
		gradients = Matrix::Scale(gradients, learningRate); // appy learning rate

		Matrix activationsLeftTransposed = Matrix::Transpose(activationsLeft);
		Matrix weightsGrad = Matrix::Product(gradients, activationsLeftTransposed);

		weights[i] = Matrix::Add(weights[i], weightsGrad);
		biases[i] = Matrix::Add(biases[i], gradients);

		// calc new errors
		Matrix weightsTransposed = Matrix::Transpose(weights[i]);

		errors = Matrix::Product(weightsTransposed, errors);
	}
}

void NeuralNetwork::SetLayerShapes(const std::vector<unsigned>& layerShapes)
{
	this->layerShapes = layerShapes;
	weights.clear();
	biases.clear();

	for (int i = 0; i < layerShapes.size() - 1; i++)
	{
		weights.push_back(Matrix(layerShapes[i], layerShapes[i + 1]));
		biases.push_back(Matrix(1, layerShapes[i + 1]));
	}
	RandomizeWeightsAndBiases();
}
