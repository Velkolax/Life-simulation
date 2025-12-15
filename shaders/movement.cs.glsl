
// SHADER THAT HANDLES MOVEMENT OF BACTERIA

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int bWidth;
uniform int bHeight;
uniform float time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

int randomInt(uint seed, int maxVal){
    float r = random(vec2(float(seed),time));
    return int(r * float(maxVal + 0.99));
}


const ivec2 evenD[6] = ivec2[](
        ivec2(0,-1), ivec2(-1,-1), ivec2(-1,0),
        ivec2(0,1), ivec2(1,0), ivec2(1,-1)
);


const ivec2 oddD[6] = ivec2[](
        ivec2(0,-1), ivec2(-1,0), ivec2(-1,1),
        ivec2(0,1), ivec2(1,1), ivec2(1,0)
);

void main(){
    uint index = gl_GlobalInvocationID.x;
    if(index >= bacteria.length()) return;
    if(bacteria[index].alive==0) return;

    ivec2 currentPos = bacteria[index].pos;
    int dir = randomInt(index,5);
    ivec2 offset;

    if((currentPos.x & 1) == 0) offset = evenD[dir];
    else offset = oddD[dir];
    if(currentPos.x<0 || currentPos.x>=bWidth || currentPos.y<0 || currentPos.y>=bHeight) {
        bacteria[index].alive = 0;
        return;
    }
    ivec2 targetPos = currentPos + offset;
    if(targetPos.x<0 || targetPos.x>=bWidth || targetPos.y<0 || targetPos.y>=bHeight) {
        return;
    }

    int currentGridIdx = currentPos.y * bWidth + currentPos.x;
    int targetGridIdx  = targetPos.y * bWidth + targetPos.x;

    if(grid[targetGridIdx] != 0 ) return;
    int myGridID = int(index) + 1;
    int prevVal = atomicCompSwap(grid[targetGridIdx],0,currentGridIdx);

    if(prevVal == 0){
        grid[currentGridIdx] = 0;
        bacteria[index].pos = targetPos;
    }
}