#version 450 core
layout(local_size_x = 64) in;

layout(std430, binding=0) buffer NetworkBuffer {
    float allWeights[];
};

uniform int stride;
uniform int paramCount;

uniform int parentAIdx;
uniform int parentBIdx;
uniform int childIdx;

uniform float mutationRate;
uniform float mutationChance;
uniform int globalSeed;
uniform int simStep;


uint hash(uint x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return x;
}

void main() {
    uint paramID = gl_GlobalInvocationID.x;
    if (paramID >= paramCount) return;


    uint addrA = paramID * stride + parentAIdx;
    uint addrB = paramID * stride + parentBIdx;
    uint addrChild = paramID * stride + childIdx;


    float weightA = allWeights[addrA];
    float weightB = allWeights[addrB];


    uint seed = hash(uint(globalSeed) + childIdx + hash(uint(simStep)));
    float rndProb = float(hash(seed)) * (1.0 / 4294967296.0);

    float rndCrossover = float(hash(seed+37)) * (1.0 / 4294967296.0);
    float newWeight = (weightA+weightB)*0.5;

    if (rndProb < mutationChance) {
        float rndVal = float(hash(seed+2137)) * (1.0 / 4294967296.0);
        float noise = (rndVal - 0.5) * 2.0 * mutationRate;
        newWeight += noise;
    }
    allWeights[addrChild] = clamp(newWeight, -5.0, 5.0);;
}