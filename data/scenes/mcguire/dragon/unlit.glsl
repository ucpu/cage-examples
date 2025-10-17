
layout(location = 0) varying vec3 normal;

$define shader vertex

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

void main()
{
	gl_Position = vec4(inPosition * 0.03, 1);
	normal = inNormal;
}

$define shader fragment

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 sunDir = normalize(vec3(1, -1, 1));
	float light = dot(sunDir, -normal);
	vec3 color = vec3(abs(light));
	outColor = vec4(color, 1);
}
