#include "NeuralNetwork.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int matrixX = 0;
int matrixY = 0;
float* matrix = NULL;




float randomFloat(float min, float max)
{
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

NeuralNetwork buildNetwork(int layerCount, int* layers)
{
    int neuronCount = 0; // exclude input layer (no biases)
    int connectionCount = 0;

    for (int i = 0; i < layerCount - 1; i++)
    {
        neuronCount += layers[i + 1];
        connectionCount += layers[i] * layers[i + 1];
    }

    float* memory = (float*)malloc((neuronCount + connectionCount) * sizeof(float));

    NeuralNetwork nn;
    nn.layerCount = layerCount;
    nn.layers = layers;
    nn.neuronCount = neuronCount;
    nn.neurons = memory;
    nn.connectionCount = connectionCount;
    nn.connections = memory + neuronCount;

    return nn;
}

NeuralNetwork childNetwork(NeuralNetwork* nn1, NeuralNetwork* nn2, float mutation)
{
    NeuralNetwork res = {0};
    if(mutation < 0. || mutation > 1.)
    {
        printf("Mutation must be a value between 0 and 1");
        return res;
    }
    if(nn1->layerCount != nn2->layerCount || memcmp(nn1->layers, nn2->layers, nn1->layerCount * sizeof(int)) != 0)
    {
        printf("Different structure of provided networks");
        return res;
    }
    res = buildNetwork(nn1->layerCount, nn1->layers);

    float mutationBottom = 1. - mutation;
    float mutationTop = 1. + mutation;

    for(int i = 0; i < nn1->neuronCount; i++)
    {
        res.neurons[i] = (nn1->neurons[i] + nn2->neurons[i]) / 2 * randomFloat(mutationBottom, mutationTop);
    }

    for(int i = 0; i < nn1->connectionCount; i++)
    {
        res.connections[i] = (nn1->connections[i] + nn2->connections[i]) / 2 * randomFloat(mutationBottom, mutationTop);
    }

    return res;
}

float* forwardPass(NeuralNetwork* nn, float* input)
{
    int L = nn->layerCount;
    int* layers = nn->layers;
    float* neurons = nn->neurons;
    float* connections = nn->connections;

    // Resize matrix if necessary
    int requiredX = 0;
    for (int i = 0; i < L; i++)
        if (layers[i] > requiredX)
            requiredX = layers[i];

    int requiredY = L;

    if (requiredX > matrixX) matrixX = requiredX;
    if (requiredY > matrixY) matrixY = requiredY;

    matrix = (float*)realloc(matrix, matrixX * matrixY * sizeof(float));

    // Copy input
    memcpy(matrix, input, layers[0] * sizeof(float));

    int neuronIndex = 0;
    int connectionIndex = 0;

    // Forward pass
    for (int y1 = 1; y1 < L; y1++)
    {
        int y0 = y1 - 1;
        int count1 = layers[y1];

        for (int x1 = 0; x1 < count1; x1++)
        {
            int idx = y1 * matrixX + x1;

            matrix[idx] = neurons[neuronIndex++]; // bias

            int count0 = layers[y0];
            for (int x0 = 0; x0 < count0; x0++)
            {
                matrix[idx] += connections[connectionIndex++] * matrix[y0 * matrixX + x0];
            }

            matrix[idx] = relu(matrix[idx]);
        }
    }

    // Copy result
    float* output = (float*)malloc(layers[L - 1] * sizeof(float));
    memcpy(output, matrix + ((L - 1) * matrixX), layers[L - 1] * sizeof(float));

    return output;
}

void initializeRandom(NeuralNetwork* nn)
{
    int neuronIndex = 0;
    int connectionIndex = 0;

    for (int i = 1; i < nn->layerCount; i++)
    {
        int n0 = nn->layers[i - 1];
        int n1 = nn->layers[i];

        float range = sqrtf(6.0f / n0);

        for (int j = 0; j < n1; j++)
            nn->neurons[neuronIndex++] = 0.01f;

        for (int j = 0; j < n1 * n0; j++)
            nn->connections[connectionIndex++] = randomFloat(-range, range);
    }
}

void printNetwork(NeuralNetwork* nn)
{
    printf("Network:\n");

    int index = 0;
    for (int y = 1; y < nn->layerCount; y++)
    {
        if (y == nn->layerCount - 1) printf("Output: ");
        else printf("Dense: ");

        for (int x = 0; x < nn->layers[y]; x++)
            printf("%f ", nn->neurons[index++]);

        printf("\n");
    }

    printf("Connections:\n");
    index = 0;

    for (int y = 1; y < nn->layerCount; y++)
    {
        int count = nn->layers[y] * nn->layers[y - 1];

        for (int x = 0; x < count; x++)
            printf("%f ", nn->connections[index++]);

        printf("\n");
    }
}

void printMatrix(NeuralNetwork* nn)
{
    printf("Matrix: %d x %d\n", matrixX, matrixY);

    for (int y = 0; y < nn->layerCount; y++)
    {
        for (int x = 0; x < nn->layers[y]; x++)
            printf("%f ", matrix[y * matrixX + x]);

        printf("\n");
    }
}

void freeNetwork(NeuralNetwork* nn)
{
    free(nn->neurons);
}