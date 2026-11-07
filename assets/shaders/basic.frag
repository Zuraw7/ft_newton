#version 330 core

uniform vec3 uColor;

in vec3 vColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(uColor * vColor, 1.0);
}
