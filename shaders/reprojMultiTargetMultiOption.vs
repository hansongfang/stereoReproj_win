#version 460 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 aNormal;

#define NUMTARGET 4
uniform mat4 curFullTransform;
uniform mat4 cacheFullTransform[NUMTARGET];
out vec4 targetPos[NUMTARGET];

// complexity
uniform uint loopx;
out vec3 temp;

// fragment-phong
uniform mat4 curModelView;
uniform mat4 curWorldView;
uniform mat4 invTransModelView;
uniform vec3 lightPos;

out vec3 vFragPos;
out vec3 vNormal;
out vec3 vLightPos;

// noise
out vec3 v_texCoord3D;

// saving depth
out vec4 fragPos;

void main()
{
    gl_Position = curFullTransform * vec4(pos, 1.0);
    fragPos = gl_Position;
    
    for(int i=0; i<NUMTARGET; i++){
        targetPos[i] = cacheFullTransform[i] * vec4(pos, 1.0);
    }
    
    // phong
    vFragPos = vec3(curModelView * vec4(pos, 1.0));
    vNormal = mat3(invTransModelView) * aNormal;
    vLightPos = vec3(curWorldView * vec4(lightPos, 1.0));//light pos from world space -> view 

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
