#version 430 core


// SHADER FOR STORING STRUCTURES, GLOBAL ARRAYS AND GLOBAL FUNCTIONS
const float SQRT_3 = 1.7320508f;

const int INPUT_SIZE = 19;
const int HIDDEN_SIZE = 16;
const int OUTPUT_SIZE = 7;

const int W1_SIZE = INPUT_SIZE * HIDDEN_SIZE;
const int B1_SIZE = HIDDEN_SIZE;
const int W2_SIZE = HIDDEN_SIZE * OUTPUT_SIZE;
const int B2_SIZE = OUTPUT_SIZE;

const int TOTAL_PARAMS = W1_SIZE + B1_SIZE + W2_SIZE + B2_SIZE;

const int NETWORK_SIZE_VEC4 = (TOTAL_PARAMS + 3) / 4;

const int W1_SIZE_F = INPUT_SIZE * HIDDEN_SIZE;
const int B1_SIZE_F = HIDDEN_SIZE;
const int W2_SIZE_F = HIDDEN_SIZE * OUTPUT_SIZE;
const int B2_SIZE_F = OUTPUT_SIZE;

const int W1_OFFSET_VEC4 = 0;
const int W1_SIZE_VEC4 = (W1_SIZE_F + 3) / 4;

const int B1_OFFSET_VEC4 = W1_OFFSET_VEC4 + W1_SIZE_VEC4;
const int B1_SIZE_VEC4 = (B1_SIZE_F + 3) / 4;

const int W2_OFFSET_VEC4 = B1_OFFSET_VEC4 + B1_SIZE_VEC4;
const int W2_SIZE_VEC4 = (W2_SIZE_F + 3) / 4;

const int B2_OFFSET_VEC4 = W2_OFFSET_VEC4 + W2_SIZE_VEC4;
const int B2_SIZE_VEC4 = (B2_SIZE_F + 3) / 4;


struct Bacteria {
    ivec2 pos;
    ivec2 target_pos;

    uint id;
    int life;
    int rem_life;
    uint alive;

    vec4 network[NETWORK_SIZE_VEC4];
};

layout(std430, binding=0) restrict buffer BacteriaBlock {
    Bacteria bacteria[];
};

layout(std430, binding = 1) restrict buffer GridBuffer {
    int grid[];
};

float ReLU(float x) {
    return max(0.0,x);
}

vec2 calculateHexPosition(int gridX, int gridY, float size, float dispX, float dispY)
{
    float height = size * SQRT_3 / 2.0f;
    float posX = gridX * size * 0.75f + dispX;
    float posY = gridY * height + dispY;
    if (gridX % 2 != 0)
    {
        posY += height / 2.0f;
    }
    return vec2(posX, posY);
}