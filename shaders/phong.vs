#version 460 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

uniform mat4 curFullTransform;
uniform mat4 curModelView;
uniform mat4 curWorldView;
uniform mat4 invTransModelView;

// uniform mat4 modelWorld;
// uniform mat4 worldView;
// uniform mat4 Projection;

uniform vec3 lightPos;

out vec3 vFragPos;
out vec3 vNormal;
out vec3 vLightPos;

out vec4 targetPos;

uniform uint loopx;
out vec3 temp;
void main()
{
	gl_Position = curFullTransform * vec4(pos.x, pos.y, pos.z, 1.0);
	targetPos = gl_Position;

	vFragPos = vec3(curModelView * vec4(pos, 1.0));
	vNormal = mat3(invTransModelView) * normal;
	vLightPos = vec3(curWorldView * vec4(lightPos, 1.0));//transform world space light position to view-space light position
	
	// loop
	vec3 temp = vec3(1.0,1.0,1.0);
	for(uint i=0; i< loopx; i++){
		for(uint j=0; j<loopx; j++){
			temp.x = mod(sqrt(i), 2.0);
		}
	}
}
