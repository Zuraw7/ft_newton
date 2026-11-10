#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

const float DEPTH_OFFSET = 0.0005;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	gl_Position.z -= DEPTH_OFFSET * gl_Position.w;
}
