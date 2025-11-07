
$define shader vertex

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inUv;

layout(std430, set = 2, binding = 0) readonly buffer UniMvpMat
{
	mat4 uniMvpMat;
};

layout(location = 0) out vec2 varUv;

void main()
{
	varUv = inUv.xy;
	gl_Position = uniMvpMat * vec4(inPosition, 1);
}

$define shader fragment

layout(location = 0) in vec2 varUv;

layout(set = 1, binding = 0) uniform sampler2D texColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(texColor, varUv);
}
