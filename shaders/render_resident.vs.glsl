layout (location = 0) in vec4 vertex;

uniform mat4 projection;
uniform vec2 viewDisplacement;
uniform float hexSize;
uniform int bWidth;
uniform int bHeight;

out vec2 TexCoords;
out vec3 HexColor;



void main() {
    uint id = gl_InstanceID;
    if(bacteria[id].alive == 0) {
        gl_Position = vec4(0.0/0.0);
        return;
    }

    vec2 worldPos = calculateHexPosition(bacteria[id].pos.x,bacteria[id].pos.y,hexSize,viewDisplacement.x,viewDisplacement.y);

    float smallSize = hexSize*0.8;
    vec2 finalPos = worldPos + (vertex.xy * smallSize);
    gl_Position = projection * vec4(finalPos, 0.0, 1.0);
    TexCoords = vertex.zw;
    HexColor = vec3(1.0,1.0,1.0);
}
