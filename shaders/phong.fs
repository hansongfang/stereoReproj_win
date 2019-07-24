#version 460 core
out vec4 color;

in vec3 vFragPos;
in vec3 vNormal;
in vec3 vLightPos;

out vec4 targetPos;

uniform vec3 uMaterialColor;
uniform vec3 uSpecularColor;
uniform vec3 uDirLightColor;
uniform vec3 uAmbientLightColor;
uniform float uKd;

uniform uint loopy;
void main()
{
	color = vec4(uAmbientLightColor * uMaterialColor, 1.0);

	// diffuse
	vec3 lVector = normalize(vLightPos); // check this lightDir, from camera to lightPos
	vec3 normal = normalize(vNormal);
	float diffuse = max(dot(normal, lVector), 0.0);
	color.xyz += uKd * uMaterialColor * uDirLightColor * diffuse;

	//loopy
	vec3 temp = vec3(1.0,1.0,1.0);
	for(uint i=0; i< loopy; i++){
		for(uint j=0; j<loopy; j++){
			temp.x = mod(sqrt(i), 2.0);
		}
	}

	vec3 targetPos_ndc = vec3(targetPos.x, targetPos.y, targetPos.z) / targetPos.w;
  	float fragDepth = (targetPos_ndc.z + 1.0) / 2.0;
	color.w = fragDepth;
}
