int GENOME_SIZE = 51;


struct Bacteria {
    // Position on hex map
    ivec2 pos;
    ivec2 target_pos;


    //vec4 network[GENOME_SIZE];
    uint id;
    //vec4 memory[2];

    // Less important parameters
    uint life;
    uint rem_life;
    //uint reflex;
    bool alive;

};


//  ---  Bacterias ---
layout(std430, binding=0) restrict buffer BacteriaBlock {
    Bacteria bacteria[];
};


// Grid but it has bacteria ID and other residents in negatives
layout(std430, binding=1) restrict buffer GridBlock {
    int grid[];
};

// Need this to find which bacteria are not active
// because counting and limits are really difficult on the gpu
layout(std430, binding=2) restrict buffer FreeBlock {
    uint freeId[];
};

layout(std430, binding=3) restrict buffer CounterBlock {
    uint aliveCount;
    uint stackTop;
    uint padding[2];
};

// >0 - Bacteria
// 0 - Empty
// -1 - water
// -2 - energy
// -3 - protein
// -4 - acid