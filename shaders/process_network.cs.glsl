#version 450 core
layout(local_size_x = 64) in;

const int INPUT = 64;
const int HIDDEN1 = 80;
const int HIDDEN2 = 32;
const int HIDDEN3 = 16;
const int OUTPUT = 8;
const int SIZE = INPUT * HIDDEN1 + HIDDEN1 + HIDDEN1 * HIDDEN2 + HIDDEN2 + HIDDEN2 * HIDDEN3 + HIDDEN3 + HIDDEN3 * OUTPUT + OUTPUT;
const int BIAS_SIZE = HIDDEN1 + HIDDEN2 + HIDDEN3 + OUTPUT;
const int WEIGHT_OFFSET = SIZE - BIAS_SIZE;

layout(std430,binding=0) readonly buffer NetworkBuffer {
    float allWeights[];
};

struct DataInOut {
    float inputs[INPUT];
    float outputs[OUTPUT];
};

/* STARY STAN */
layout(std430, binding=1) readonly buffer InBuffer {
    DataInOut inData[];
};


/* NOWY STAN */
layout(std430, binding=2) writeonly buffer OutBuffer {
    DataInOut outData[];
};

void main() {

}