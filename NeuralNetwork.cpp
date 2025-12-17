#include "NeuralNetwork.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <random>
#include <time.h>

#include "BacteriaData.h"


int matrixX = 0;
int matrixY = 0;
float* matrix = NULL;

int resultBufforSize = 0;
float* resultBuffor = NULL;


/*NeuralNetwork buildNetwork(int layerCount, int* layers)
{
    float* memory = (float*)malloc((neuronCount + connectionCount) * sizeof(float));

    NeuralNetwork nn;
    nn.neuronCount = neuronCount;
    nn.neurons = memory;
    nn.connectionCount = connectionCount;
    nn.connections = memory + neuronCount;

    return nn;
}*/

NeuralNetwork childNetwork(NeuralNetwork* nn1, NeuralNetwork* nn2, float mutation)
{
    NeuralNetwork res;

    float mutationBottom = 1. - mutation;
    float mutationTop = 1. + mutation;
    std::uniform_real_distribution<float> mutationDist(mutationBottom, mutationTop);

    for(int i = 0; i < BIASES; i++)
    {
        res.biases[i] = (nn1->biases[i] + nn2->biases[i]) / 2 * mutationDist(gen);
    }

    for(int i = 0; i < CONNECTIONS; i++)
    {
        res.connections[i] = (nn1->connections[i] + nn2->connections[i]) / 2 * mutationDist(gen);
    }

    return res;
}

void NeuralNetwork::initializeRandom()
{
    int connectionIndex = 0;

    for (int i = 0; i < BIASES; i++) biases[i] = 0.01f;

    for (int i = 1; i < layersSize; i++)
    {
        int n0 = layers[i - 1];
        int n1 = layers[i];

        float range = sqrtf(6.0f / n0);
        std::uniform_real_distribution<float> connectionsDist(-range, range);

        for (int j = 0; j < n1 * n0; j++)
            connections[connectionIndex++] = connectionsDist(gen);
    }
}

void NeuralNetwork::printNetwork()
{
    printf("Network:\n");

    int index = 0;
    for (int y = 1; y < layersSize; y++)
    {
        if (y == layersSize - 1) printf("Output: ");
        else printf("Dense: ");

        for (int x = 0; x < layers[y]; x++)
            printf("%f ", biases[index++]);

        printf("\n");
    }

    printf("Connections:\n");
    index = 0;

    for (int y = 1; y < layersSize; y++)
    {
        int count = layers[y] * layers[y - 1];

        for (int x = 0; x < count; x++)
            printf("%f ", connections[index++]);

        printf("\n");
    }
}