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
uniform float seed;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    uint bacteriaID = gl_GlobalInvocationID.x;
    uint paramLocalID = gl_GlobalInvocationID.y;

    if (bacteriaID >= stride || paramLocalID >= paramCount) return;

    uint globalParamIdx = paramOffset + paramLocalID;

    uint memoryIndex = globalParamIdx * stride + bacteriaID;

    vec2 noiseSeed = vec2(float(bacteriaID) * 0.123 + seed, float(globalParamIdx) * 0.456);
    float rnd = random(noiseSeed);

    allWeights[memoryIndex] = minVal + rnd * (maxVal - minVal);
}