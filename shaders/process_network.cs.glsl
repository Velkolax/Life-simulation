#version 450 core
layout(local_size_x = 64) in;

const int INPUT = 64;
const int HIDDEN1 = 80;
const int HIDDEN2 = 32;
const int HIDDEN3 = 16;
const int OUTPUT = 16;

const int DIRECTIONS=7;
const int MEMORY=3;
const int PARAMETERS=4;
const int PARAMETER_START = MEMORY + DIRECTIONS;


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

float erf(float x) {
    float s = sign(x);
    float a = abs(x);
    float t = 1.0 / (1.0 + 0.3275911 * a);
    float y = 1.0 - (((((1.061405429 * t + -1.453152027) * t) + 1.421413741) * t + -0.284496736) * t + 0.254829592) * t * exp(-a * a);
    return s * y;
}

uint hash(uint x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return x;
}

uniform int activeBacteria;
uniform int stride;
uniform int indices;
uniform int globalSeed;
uniform int simStep;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= indices) return;
    uint id = ids[index];
    if(id >= stride) return;

    uint seed = hash(uint(globalSeed) + index + hash(uint(simStep)));
    float r2 = float(hash(seed + 12345u)) * (1.0 / 4294967296.0);

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
        h1[i] = tanh(sum);
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
        h2[i] = tanh(sum);
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
        h3[i] = tanh(sum);
    }

    float rawOutputs[OUTPUT];

    for(int i=0; i<OUTPUT; i++) {
        float sum = allWeights[bPtr * stride + id];
        bPtr++;
        for(int j=0; j<HIDDEN3; j++) {
            float weight = allWeights[wPtr * stride + id];
            sum += h3[j] * weight;
            wPtr++;
        }
        rawOutputs[i] = sum;
    }

    float maxLogit = rawOutputs[MEMORY];
    for(int i=1;i<DIRECTIONS;i++) maxLogit = max(maxLogit,rawOutputs[MEMORY+i]);

    float sumExp = 0.0;
    float probs[DIRECTIONS];
    for(int i=0; i<DIRECTIONS; i++) {
        probs[i] = exp(rawOutputs[MEMORY+i] - maxLogit);
        sumExp += probs[i];
    }
    for(int i=0; i<DIRECTIONS; i++) probs[i] /= sumExp;

    int chosenDirection = 0;
    float cumulativeProb = 0.0;
    for(int i=0; i<DIRECTIONS; i++) {
        cumulativeProb += probs[i];
        if(r2 <= cumulativeProb) {
            chosenDirection = i;
            break;
        }
    }


    for(int i=0;i<MEMORY;i++)
        outData[index][i] = (tanh(rawOutputs[i])+1)*0.5;

    outData[index][MEMORY] = float(chosenDirection) / float(DIRECTIONS-1);

    for(int i=0;i<PARAMETERS;i++)
        outData[index][i+MEMORY+1] = (tanh(rawOutputs[i+PARAMETER_START])+1)*0.5;
}