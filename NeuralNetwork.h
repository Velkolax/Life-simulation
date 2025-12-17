#pragma once

#include <iterator>
#include <stdio.h>

// ALL NETWORKS SHOULD HAVE THIS SIZE
constexpr size_t INPUT = 64;
constexpr size_t HIDDEN1 = 80;
constexpr size_t HIDDEN2 = 32;
constexpr size_t HIDDEN3 = 16;
constexpr size_t OUTPUT = 8;
constexpr size_t BIASES = HIDDEN1 + HIDDEN2 + HIDDEN3 + OUTPUT;
constexpr size_t CONNECTIONS = INPUT * HIDDEN1 + HIDDEN1 * HIDDEN2 + HIDDEN2 * HIDDEN3 + HIDDEN3 * OUTPUT;
constexpr size_t SIZE = BIASES + CONNECTIONS;

inline constexpr int layers[] = {INPUT, HIDDEN1, HIDDEN2, HIDDEN3, OUTPUT};
inline constexpr int layersSize = std::size(layers);


struct alignas(16) DataInOut
{
    float input[INPUT];
    float output[OUTPUT];
};

constexpr size_t INOUT_SIZE = sizeof(DataInOut);


typedef struct
{
    float biases[BIASES];
    float connections[CONNECTIONS];

    void printNetwork();
    void initializeRandom();
} NeuralNetwork;

static inline float relu(float value) { return value * (value > 0); }

// Merge networks
NeuralNetwork childNetwork(NeuralNetwork* nn1, NeuralNetwork* nn2, float mutation);

