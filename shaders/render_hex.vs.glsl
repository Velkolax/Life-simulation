layout (location = 0) in vec4 vertex;

uniform mat4 projection;
uniform vec2 viewDisplacement;
uniform float hexSize;
uniform int bWidth;
uniform int bHeight;

out vec2 TexCoords;
out vec3 HexColor;



void main() {
    int id = gl_InstanceID;
    if(grid[id] == -1 ) {
        gl_Position = vec4(0.0/0.0);
        return;
    }
    int y = id/bWidth;
    int x = id % bWidth;
    vec2 worldPos = calculateHexPosition(x,y,hexSize,viewDisplacement.x,viewDisplacement.y);
    vec2 finalPos =worldPos + (vertex.xy * hexSize);
    gl_Position = projection * vec4(finalPos, 0.0, 1.0);
    TexCoords = vertex.zw;
    HexColor = vec3(1.0,1.0,1.0);

}

