#pragma once

#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>

    typedef struct
    {
        int neuronCount;        // without input layer (no biases there)
        float* neurons;
        int connectionCount;
        float* connections;
    } NeuralNetwork;

    extern int matrixX;
    extern int matrixY;
    extern float* matrix;
    static int* globalLayers;

    static inline float relu(float value) { return value * (value > 0); }

    // Build network
    NeuralNetwork buildNetwork(int layerCount, int* layers);

    // Merge networks
    NeuralNetwork childNetwork(NeuralNetwork* nn1, NeuralNetwork* nn2, float mutation);

    // Forward pass
    float* forwardPass(NeuralNetwork* nn, float* input);

    // Initialization
    void initializeRandom(NeuralNetwork* nn);

    // Debug printout
    void printNetwork(NeuralNetwork* nn);
    void printMatrix(NeuralNetwork* nn);

    // Free memory
    void freeNetwork(NeuralNetwork* nn);
#endif


