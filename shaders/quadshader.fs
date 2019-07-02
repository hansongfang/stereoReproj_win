#version 460 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D screenTexture;
float LinearizeDepth(in vec2 uv)
{
    float zNear = 1.0;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 100.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(screenTexture, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
	//float c =  LinearizeDepth(TexCoords);
	//color = vec4(c, c, c, 1.0);
	
	color = texture2D(screenTexture, TexCoords);
}