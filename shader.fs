#version 450 core

in vec2 TextCoords;
in vec3 FragPosition;
in vec3 Normal;

out vec4 fragmentColor;

uniform sampler2D earth;
uniform sampler2D specularMap;
uniform sampler2D clouds;
uniform sampler2D merkurius;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 view;

//For point light
uniform float constant;
uniform float linear;
uniform float quadratic;

void main()
{
	// Cahaya Sekeliling Lighting
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * vec3(texture(clouds, TextCoords));

	// Penyebaran Cahaya
    vec3 LightPos = vec3(view * vec4(lightPos, 1.0));
	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPosition);
    float diff = max(dot(norm, lightDir), 0.3);
    vec3 diffuse = diff * vec3(texture(earth, TextCoords));

    // Munculnya titik cahaya di permukaan mengkilap
    float specularStrength = 0.3;
    vec3 ViewPos = vec3(view * vec4(viewPos, 1.0));
    vec3 viewDir = normalize(ViewPos - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(texture(specularMap, TextCoords));

	// Titik cahaya
	float distance = length(LightPos - FragPosition);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	// hasil gabungan
	fragmentColor = vec4(attenuation * (ambient + diffuse + specular), 1.0);
}
