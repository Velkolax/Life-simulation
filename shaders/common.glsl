#version 430 core


// SHADER FOR STORING STRUCTURES, GLOBAL ARRAYS AND GLOBAL FUNCTIONS
const float SQRT_3 = 1.7320508f;

const int INPUT_SIZE = 6;
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
const ivec2 evenD[6] = ivec2[](
ivec2(0,-1), ivec2(-1,-1), ivec2(-1,0),
ivec2(0,1), ivec2(1,0), ivec2(1,-1)
);


const ivec2 oddD[6] = ivec2[](
ivec2(0,-1), ivec2(-1,0), ivec2(-1,1),
ivec2(0,1), ivec2(1,1), ivec2(1,0)
);

float ReLU(float x) {
    return max(0.0,x);
}

float getWeight(uint b_id, int offset_vec4, int linear_idx) {
    int vec_idx = offset_vec4 + (linear_idx >> 2);
    int comp_idx = linear_idx & 3;
    vec4 v = bacteria[b_id].network[vec_idx];
    return v[comp_idx];
}

int computeNetwork(uint b_id,ivec2 currentPos,int bWidth,int bHeight) {
    float inputs[INPUT_SIZE];

    for (int i = 0; i < INPUT_SIZE; ++i) {
        ivec2 offset;
        if ((currentPos.x & 1) == 0) offset = evenD[i];
        else offset = oddD[i];
        ivec2 targetPos = currentPos + offset;
        int targetGridIdx = targetPos.y * bWidth + targetPos.x;
        if (targetPos.x < 0 || targetPos.x >= bWidth || targetPos.y < 0 || targetPos.y >= bHeight) inputs[i] = -1;
        else{
            int val = grid[targetGridIdx];
            inputs[i] = (val != 0) ? 1.0 : 0.0;
        }
    }

    float hidden[HIDDEN_SIZE];

    for (int h = 0; h < HIDDEN_SIZE; ++h) {
        float sum = 0.0;
        sum += getWeight(b_id, B1_OFFSET_VEC4, h);
        for (int in_idx = 0; in_idx < INPUT_SIZE; ++in_idx) {
            int weight_idx = in_idx * HIDDEN_SIZE + h;
            float weight = getWeight(b_id, W1_OFFSET_VEC4, weight_idx);
            sum += inputs[in_idx] * weight;
        }
        hidden[h] = ReLU(sum);
    }
    float outputs[OUTPUT_SIZE];

    for (int o = 0; o < OUTPUT_SIZE; ++o) {
        float sum = 0.0;
        sum += getWeight(b_id, B2_OFFSET_VEC4, o);
        for (int h = 0; h < HIDDEN_SIZE; ++h) {
            int weight_idx = h * OUTPUT_SIZE + o;
            float weight = getWeight(b_id, W2_OFFSET_VEC4, weight_idx);
            sum += hidden[h] * weight;
        }
        outputs[o] = sum;
    }
    float maks = -10000;
    int argmax = 2;
    for(int i=0;i<OUTPUT_SIZE;i++){
        if(outputs[i]>maks){
            maks = outputs[i];
            argmax = i;
        }
    }
    return argmax;
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