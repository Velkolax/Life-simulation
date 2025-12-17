#version 450 core
layout(local_size_x = 64) in;

const int INPUT = 64;
const int HIDDEN1 = 80;
const int HIDDEN2 = 32;
const int HIDDEN3 = 16;
const int OUTPUT = 8;


const int SIZE = INPUT * HIDDEN1 + HIDDEN1 + HIDDEN1 * HIDDEN2 + HIDDEN2 + HIDDEN2 * HIDDEN3 + HIDDEN3 + HIDDEN3 * OUTPUT + OUTPUT;
const int BIAS_SIZE = HIDDEN1 + HIDDEN2 + HIDDEN3 + OUTPUT;


const int B1_START = 0;
const int W1_START = BIAS_SIZE;

layout(std430, binding=0) readonly buffer NetworkBuffer {
    float allWeights[];
};

layout(std430, binding=1) readonly buffer InBuffer {
    float inData[][INPUT];
};

layout(std430, binding=2) buffer OutBuffer {
    float outData[][OUTPUT];
};

layout(std430, binding=3) buffer IdBuffer {
    uint ids[];
};

float relu(float x){
    return max(0.0, x);
}

uniform int activeBacteria;
uniform int stride;
uniform int indices;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= indices) return;
    uint id = ids[index];
    if(id >= stride) return;

    float inputLayer[INPUT];
    for(int i=0; i<INPUT; i++) inputLayer[i] = inData[index][i];

    int bPtr = B1_START;
    int wPtr = W1_START;


    float h1[HIDDEN1];
    for(int i=0; i<HIDDEN1; i++) {
        float sum = allWeights[bPtr * stride + id];
        bPtr++;

        for(int j=0; j<INPUT; j++) {
            float weight = allWeights[wPtr * stride + id];
            sum += inputLayer[j] * weight;
            wPtr++;
        }
        h1[i] = relu(sum);
    }

    float h2[HIDDEN2];
    for(int i=0; i<HIDDEN2; i++) {
        float sum = allWeights[bPtr * stride + id];
        bPtr++;
        for(int j=0; j<HIDDEN1; j++) {
            float weight = allWeights[wPtr * stride + id];
            sum += h1[j] * weight;
            wPtr++;
        }
        h2[i] = relu(sum);
    }

    float h3[HIDDEN3];
    for(int i=0; i<HIDDEN3; i++) {
        float sum = allWeights[bPtr * stride + id];
        bPtr++;
        for(int j=0; j<HIDDEN2; j++) {
            float weight = allWeights[wPtr * stride + id];
            sum += h2[j] * weight;
            wPtr++;
        }
        h3[i] = relu(sum);
    }

    for(int i=0; i<OUTPUT; i++) {
        float sum = allWeights[bPtr * stride + id];
        bPtr++;
        for(int j=0; j<HIDDEN3; j++) {
            float weight = allWeights[wPtr * stride + id];
            sum += h3[j] * weight;
            wPtr++;
        }
        outData[index][i] = tanh(sum);
    }

     //outData[id].outputs[0] = 123.456;
     //outData[id].outputs[1] = float(id);
}