#version 330

uniform vec3 uCameraPos;
uniform float uFadeStart;
uniform float uFadeEnd;

in vec3 vWorldPos;

out vec4 FragColor;

const vec3 MINOR_COLOR = vec3(0.35);
const vec3 MAJOR_COLOR = vec3(0.15);
const vec3 AXIS_X_COLOR = vec3(0.9, 0.2, 0.2);
const vec3 AXIS_Z_COLOR = vec3(0.05, 0.15, 0.75);


float gridLine(vec2 coord, float spacing)
{
	vec2 scaled = coord / spacing;
	vec2 pixelSize = fwidth(scaled);
	vec2 dist = abs(fract(scaled - 0.5) - 0.5) / pixelSize;
	return 1.0 - min(min(dist.x, dist.y), 1.0);
}

float axisLine(float value)
{
	return 1.0 - min(abs(value) / fwidth(value), 1.0);
}

void main()
{
	vec2 pos = vWorldPos.xz;

	float minor = gridLine(pos, 1.0);
	float major = gridLine(pos, 10.0);

	vec3 color = MINOR_COLOR;
	float alpha = minor * 0.5;

	if (major > 0.0) {
		color = MAJOR_COLOR;
		alpha = max(alpha, major * 0.8);
	}

	float axisX = axisLine(pos.y);
	float axisZ = axisLine(pos.x);

	if (axisX > 0.0) {
		color = AXIS_X_COLOR;
		alpha = max(alpha, axisX);
	}

	if (axisZ > 0.0) {
		color = AXIS_Z_COLOR;
		alpha = max(alpha, axisZ);
	}

	float distanceToCamera = length(vWorldPos - uCameraPos);
	alpha *= 1.0 - smoothstep(uFadeStart, uFadeEnd, distanceToCamera);

	FragColor = vec4(color, alpha);
}
