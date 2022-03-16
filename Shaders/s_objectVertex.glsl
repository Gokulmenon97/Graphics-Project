#version 330 core

in vec3 position;
in vec3 normal; // New Attribute !
in vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out Vertex
{
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal;
} OUT;

void main(void)
{
	vec4 fragPos = (modelMatrix * vec4(position,1));
	OUT.texCoord = texCoord;
	OUT.normal = mat3(transpose(inverse(modelMatrix))) * normal;
	OUT.fragPos = vec3(fragPos);
	vec4 result = (projMatrix * viewMatrix) * fragPos;
	gl_Position = result;
}