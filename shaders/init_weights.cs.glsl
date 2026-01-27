#version 450 core
layout(local_size_x = 64, local_size_y = 1) in;

layout(std430, binding=0) writeonly buffer NetworkBuffer {
    float allWeights[];
};

layout(std430, binding=1) buffer SpeciesBuffer {
    uint allSpecies[];
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
    uint trueID = bacteriaID/paramCount;
    if (bacteriaID >= stride || paramLocalID >= paramCount) return;

    uint globalParamIdx = paramOffset + paramLocalID;
    uint memoryIndex = globalParamIdx * stride + bacteriaID;

    uint baseSeed = hash(hash(allSpecies[bacteriaID]) + hash(globalParamIdx));
    uint addedSeed = hash(uint(globalSeed) + bacteriaID + hash(paramLocalID + paramOffset));
    float baseRnd = float(hash(baseSeed)) * (1.0 / 4294967296.0);
    float addedRnd = float(hash(addedSeed)) * (1.0 / 4294967296.0);
    allWeights[memoryIndex] = minVal + baseRnd * (maxVal-minVal) * 0.9 + addedRnd * (maxVal - minVal) * 0.1;
    //allWeights[memoryIndex] = float(allSpecies[trueID]);
}