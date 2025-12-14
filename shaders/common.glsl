int LAYER_SIZE = 27;


struct Bacteria {
    // Position on hex map
    vec2 position;


    vec4 network[LAYER_SIZE];
    vec4 memory[2];

    // Less important parameters
    uint lifespan;
    uint energy;
    uint remainingLife;
    uint remainingEnergy;

};


//  ---  Bacterias ---
layout(std430, binding=0) restrict buffer BacteriaBlock {
    Bacteria bacteria[];
};


// Grid but it has bacteria ID and other residents in negatives
layout(std430, binding=1) restrict buffer GridBlock {
    int grid[];
};

// >0 - Bacteria
// 0 - Empty
// -1 - water
// -2 - energy
// -3 - protein
// -4 - acid