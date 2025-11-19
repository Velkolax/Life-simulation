#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>

typedef struct
{
    int layerCount;
    int* layers;
    int neuronCount;        // without input layer (no biases there)
    float* neurons;
    int connectionCount;
    float* connections;
} NeuralNetwork;

extern int matrixX;
extern int matrixY;
extern float* matrix;

// Activation
inline float relu(float value) { return value * (value > 0); }

NeuralNetwork CrossOver(NeuralNetwork nn1, NeuralNetwork nn2);
static NeuralNetwork MutateNetwork(NeuralNetwork nn);
// Random
float randomFloat(float min, float max);

// Build network
NeuralNetwork buildNetwork(int layerCount, int* layers);

// Forward pass
float* forwardPass(NeuralNetwork* nn, float* input);

// Initialization
void initializeRandom(NeuralNetwork* nn);

// Debug printout
void printNetwork(NeuralNetwork* nn);
void printMatrix(NeuralNetwork* nn);

#endif
