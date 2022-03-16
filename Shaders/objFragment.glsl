#version 330 core

out vec4 fragColour;



in Vertex{
	vec2 texCoord;
	vec4 fragPos;
	vec3 normal;
} IN;

struct Material{
sampler2D diffuseTex;
};

struct Light{
vec3 direction;
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main(void)
{
	//ambient
	vec3 ambient = light.ambient * texture(material.diffuseTex, IN.texCoord).rgb;
	
	//diffuse
	vec3 norm = normalize(IN.normal);
	vec3 lightDir = normalize(-light.direction);
	float diffuseImpact = max(dot(norm, lightDir), 0.0);
	vec3 diffuseResult = light.diffuse * diffuseImpact * texture(material.diffuseTex, IN.texCoord).rgb;
	
	//specular
	
	vec3 result = ambient + diffuseResult;
	fragColour = vec4(result, 1.0);
}