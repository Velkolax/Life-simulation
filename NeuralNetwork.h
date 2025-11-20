
#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>

/* To mówi kompilatorowi C++: "Nie zmieniaj nazw tych funkcji, one są z C" */
#ifdef __cplusplus
extern "C" {
#endif

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
    // ZMIANA: dodano 'static'. Dzięki temu funkcja jest widoczna w każdym pliku,
    // który includuje ten nagłówek, bez błędu linkera.
    static inline float relu(float value) { return value * (value > 0); }

    // Random
    float randomFloat(float min, float max);

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

#ifdef __cplusplus
}
#endif

#endif


