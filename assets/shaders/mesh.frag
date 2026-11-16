#version 330 core

uniform vec3 uColor;

in vec3 vNormal;

out vec4 FragColor;

const vec3 LIGHT_DIR = normalize(vec3(0.4, 1.0, 0.3));
const float AMBIENT = 0.25;

void main()
{
	float diffuse = max(dot(normalize(vNormal), LIGHT_DIR), 0.0);
	FragColor = vec4(uColor * (AMBIENT + (1.0 - AMBIENT) * diffuse), 1.0);
}
