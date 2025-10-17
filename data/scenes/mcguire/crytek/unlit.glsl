
layout(location = 0) varying vec3 varNormal;
layout(location = 1) varying vec2 varUv;

$define shader vertex

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 4) in vec2 inUv;

void main()
{
	gl_Position = vec4(inPosition * 0.03, 1);
	varNormal = inNormal;
	varUv = inUv;
}

$define shader fragment

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D texAlbedo;
layout(set = 1, binding = 3) uniform sampler2D texSpecial;
layout(set = 1, binding = 5) uniform sampler2D texNormal;
layout(set = 1, binding = 7) uniform sampler2D texCustom;

void main()
{
	vec4 albedo = texture(texAlbedo, varUv);
	vec3 sunDir = normalize(vec3(1, -1, 1));
	float light = dot(sunDir, -varNormal);
	vec3 color = vec3(abs(light)) * vec3(albedo);
	outColor = vec4(color, 1);
}
