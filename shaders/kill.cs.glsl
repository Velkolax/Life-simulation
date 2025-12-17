#version 450 core
layout(local_size_x = 64) in;


layout(std430, binding=0) buffer NetworkBuffer {
    float allWeights[];
};

uniform int stride;
uniform int paramCount;


uniform int deadIdx;
uniform int lastIdx;

void main() {
    uint paramID = gl_GlobalInvocationID.x;
    if (paramID >= paramCount) return;


    uint srcAddr = paramID * stride + lastIdx;
    uint dstAddr = paramID * stride + deadIdx;


    allWeights[dstAddr] = allWeights[srcAddr];
}