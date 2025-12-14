#version 430 core
layout (location = 0) in vec4 vertex;

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
    uint alive;

};


//  ---  Bacterias ---
layout(std430, binding=0) buffer BacteriaBlock {
    Bacteria bacteria[];
};


const float SQRT_3 = 1.7320508f;

uniform mat4 projection;
uniform vec2 viewDisplacement;
uniform float hexSize;
uniform int bWidth;
uniform int bHeight;

out vec2 TexCoords;
out vec3 HexColor;

vec2 calculateHexPosition(int gridX, int gridY, float size)
{
    float height = size * SQRT_3 / 2.0f;
    float posX = gridX * size * 0.75f + viewDisplacement.x;
    float posY = gridY * height + viewDisplacement.y;
    if (gridX % 2 != 0)
    {
        posY += height / 2.0f;
    }
    return vec2(posX, posY);
}

void main() {
    uint id = gl_InstanceID;
    if(bacteria[id].alive == 0) {
        gl_Position = vec4(0.0/0.0);
    }

    vec2 worldPos = calculateHexPosition(bacteria[id].pos.x,bacteria[id].pos.y,hexSize);

    float smallSize = hexSize*0.8;
    vec2 finalPos = worldPos + (vertex.xy * smallSize);
    gl_Position = projection * vec4(finalPos, 0.0, 1.0);
    TexCoords = vertex.zw;
    HexColor = vec3(1.0,1.0,1.0);
}
