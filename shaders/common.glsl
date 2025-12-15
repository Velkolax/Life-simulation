#version 430 core

const float SQRT_3 = 1.7320508f;

struct Bacteria {
    // Position on hex map
    ivec2 pos;
    ivec2 target_pos;


    //vec4 network[GENOME_SIZE];
    uint id;
    //vec4 memory[2];

    // Less important parameters
    int life;
    int rem_life;
    //uint reflex;
    uint alive;

};

layout(std430, binding=0) restrict buffer BacteriaBlock {
    Bacteria bacteria[];
};

layout(std430, binding = 1) restrict buffer GridBuffer {
    int grid[];
};

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