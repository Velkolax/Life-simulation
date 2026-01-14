#version 450 core
layout(local_size_x = 64, local_size_y = 1) in;

layout(std430, binding=0) writeonly buffer NetworkBuffer {
    float allWeights[];
};

uniform int stride;
uniform int paramOffset;
uniform int paramCount;
uniform float minVal;
uniform float maxVal;
uniform int globalSeed;

uint hash(uint x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return x;
}

void main() {
    uint bacteriaID = gl_GlobalInvocationID.x;
    uint paramLocalID = gl_GlobalInvocationID.y;

    if (bacteriaID >= stride || paramLocalID >= paramCount) return;

    uint globalParamIdx = paramOffset + paramLocalID;
    uint memoryIndex = globalParamIdx * stride + bacteriaID;

    uint seed = hash(uint(globalSeed) + bacteriaID + hash(paramLocalID + paramOffset));
    float rnd = float(hash(seed)) * (1.0 / 4294967296.0);
    allWeights[memoryIndex] = minVal + rnd * (maxVal - minVal);
}