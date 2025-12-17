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
uniform float seed;


float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    uint paramID = gl_GlobalInvocationID.x;
    if (paramID >= paramCount) return;


    uint addrA = paramID * stride + parentAIdx;
    uint addrB = paramID * stride + parentBIdx;
    uint addrChild = paramID * stride + childIdx;


    float weightA = allWeights[addrA];
    float weightB = allWeights[addrB];
    float newWeight = (weightA + weightB) * 0.5;


    float rndProb = random(vec2(paramID, seed));

    if (rndProb < mutationChance) {
        float rndVal = random(vec2(seed, paramID));
        float noise = (rndVal - 0.5) * 2.0 * mutationRate;
        newWeight += noise;
    }

    // Zapis do slotu dziecka
    allWeights[addrChild] = newWeight;
}