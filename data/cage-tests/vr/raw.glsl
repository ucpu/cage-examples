
$include /cage/shaders/shaderConventions.h

$define shader vertex

layout(location = CAGE_SHADER_ATTRIB_IN_POSITION) in vec3 inPosition;
layout(location = CAGE_SHADER_ATTRIB_IN_UV) in vec3 inUv;

layout(location = 0) uniform mat4 uniMvpMat;

out vec2 varUv;

void main()
{
	varUv = inUv.xy;
	gl_Position = uniMvpMat * vec4(inPosition, 1);
}

$define shader fragment

in vec2 varUv;

layout(binding = 0) uniform sampler2D texColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(texColor, varUv);
}
