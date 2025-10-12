
$define shader vertex

layout(location = 0) in vec3 inPosition;

void main()
{
	gl_Position = vec4(inPosition, 1);
}


$define shader fragment

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(0.4, 0.4, 0.4, 1);
}
