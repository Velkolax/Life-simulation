#version 450 core
layout(local_size_x = 64) in;

const int INPUT = 64;
const int HIDDEN1 = 80;
const int HIDDEN2 = 32;
const int HIDDEN3 = 16;
const int OUTPUT = 8;


const int SIZE = INPUT * HIDDEN1 + HIDDEN1 + HIDDEN1 * HIDDEN2 + HIDDEN2 + HIDDEN2 * HIDDEN3 + HIDDEN3 + HIDDEN3 * OUTPUT + OUTPUT;
const int BIAS_SIZE = HIDDEN1 + HIDDEN2 + HIDDEN3 + OUTPUT;

/* OFFSETY WAG */
const int W1_START = BIAS_SIZE;
const int W2_START = W1_START + (INPUT * HIDDEN1);
const int W3_START = W2_START + (HIDDEN1 * HIDDEN2);
const int W_OUT_START = W3_START + (HIDDEN2 * HIDDEN3);

/* OFFSETY BIASÓW */
const int B1_START = 0;
const int B2_START = B1_START + HIDDEN1;
const int B3_START = B2_START + HIDDEN2;
const int B_OUT_START = B3_START + HIDDEN3;



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

float relu(float x){
    return max(0.0,x);
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= inData.length()) return;

    /* WCZYTANIE WEJŚĆ */
    uint basePtr = id * SIZE;
    float inputLayer[INPUT];
    for(int i=0;i<INPUT;i++) inputLayer[i] = inData[id].inputs[i];

    uint wPtr = basePtr + W1_START;
    uint bPtr = basePtr + B1_START;

    /* PRZETWARZANIE SIECI */
    float h1[HIDDEN1];
    for(int i=0;i<HIDDEN1;i++) {
        float sum = allWeights[bPtr++];
        for(int j=0;j<INPUT;j++) sum += inputLayer[j] * allWeights[wPtr++];
        h1[i] = relu(sum);
    }

    float h2[HIDDEN2];
    for(int i=0;i<HIDDEN2;i++) {
        float sum = allWeights[bPtr++];
        for(j=0;j<HIDDEN1;j++) sum += h1[j] * allWeights[wPtr++];
        h2[i] = relu(sum);
    }

    float h3[HIDDEN3];
    for(int i=0;i<HIDDEN3;i++) {
        float sum = allWeights[bPtr++];
        for(int j=0;j<HIDDEN2;j++) sum += h2[j] * allWeights[wPtr++];
        h3[i] = relu(sum);
    }

    for(int i=0;i<OUTPUT;i++) {
        float sum = allWeights[bPtr++];
        for(int j=0;j<HIDDEN3;j++) sum += h3[j] * allWeights[wPtr++];
        outData[id].outputs[i] = tanh(sum);
    }


}