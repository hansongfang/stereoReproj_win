#version 460 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 aNormal;

uniform mat4 curFullTransform;
uniform mat4 cacheFullTransform;
out vec4 targetPos;

// complexity
uniform uint loopx;
out vec3 temp;

// fragment-phong
out vec3 vPos;
out vec3 vaNormal;

// noise
out vec3 v_texCoord3D;

void main()
{
    gl_Position = curFullTransform * vec4(pos, 1.0);
    targetPos = cacheFullTransform * vec4(pos, 1.0);

    // phong
    vPos = pos;
    vaNormal = aNormal;

    // noise
    v_texCoord3D =  pos;

    // loop
    vec3 temp = vec3(1.0,1.0,1.0);
    for(uint i=0; i< loopx; i++){
        for(uint j=0; j<loopx; j++){
           temp.x = mod(sqrt(i), 2.0);
        }
    }
}
