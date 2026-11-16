#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

const float TOWARDS_CAMERA = 0.001;

void main()
{
	vec4 viewPos = uView * uModel * vec4(aPos, 1.0);
	viewPos.xyz *= 1.0 - TOWARDS_CAMERA;

	gl_Position = uProjection * viewPos;
}
