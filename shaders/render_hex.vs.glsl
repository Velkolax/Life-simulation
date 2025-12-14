#version 430 core
layout (location = 0) in vec4 vertex;

// Grid but it has bacteria ID and other residents in negatives
layout(std430, binding=1) restrict buffer GridBlock {
    int grid[];
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
    int id = gl_InstanceID;
    if(grid[id] == -1 ) {
        gl_Position = vec4(0.0/0.0);
        return;
    }
    int y = id/bWidth;
    int x = id % bWidth;
    vec2 worldPos = calculateHexPosition(x,y,hexSize);
    vec2 finalPos =worldPos + (vertex.xy * hexSize);
    gl_Position = projection * vec4(finalPos, 0.0, 1.0);
    TexCoords = vertex.zw;
    HexColor = vec3(1.0,1.0,1.0);

}

