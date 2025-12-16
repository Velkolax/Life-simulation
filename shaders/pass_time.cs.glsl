layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int dt;

void main(){
    uint index = gl_GlobalInvocationID.x;
//    if(bacteria[index].alive==0) return;

    bacteria[index].rem_life-=dt;

    if(bacteria[index].rem_life<=0) bacteria[index].alive=0;
}