#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuseTex;
    //sampler2D specularTex; 
	//sampler2D bumpTex;
    float shininess;
}; 

struct Light {
    vec3 position;
	vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in Vertex
{
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal;
} IN;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	//ambient
    vec3 ambientResult = light.ambient * texture(material.diffuseTex, IN.texCoord).rgb;
	
	//diffuse 
    vec3 norm = normalize(IN.normal);
    vec3 lightDir = normalize(light.position - IN.fragPos);
    float diffuseImpact = max(dot(norm, lightDir), 0.0);
    vec3 diffuseResult = light.diffuse * diffuseImpact * texture(material.diffuseTex, IN.texCoord).rgb;
	
	//specular
	vec3 viewDir = normalize(viewPos - IN.fragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * specFactor * texture(material.specularTex, IN.texCoord).rgb;
	
	//spotlight (soft edges)
    float theta = dot(lightDir, normalize(-viewDir)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuseResult  *= intensity;
    //specular *= intensity;
	
	//attenuation
    float distance    = length(light.position - IN.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambientResult  *= attenuation; 
    diffuseResult   *= attenuation;
    //specular *= attenuation;
	
	vec3 result = ambientResult + diffuseResult;// + specular;
    FragColor = vec4(result, 1.0);
} 