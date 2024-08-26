
$include /cage/shader/shaderConventions.h

$include /cage/shader/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shader/engine/fragment.glsl

layout(binding = CAGE_SHADER_TEXTURE_CUSTOM) uniform sampler2D texPostules;

void main()
{
#ifndef DepthOnly
	updateNormal();
	Material material = loadMaterial();
	float postule = texture(texPostules, varUv.xy).r;
	material.albedo = mix(material.albedo, vec3(0.5, 1, 0), postule);
	outColor = lighting(material);
#endif // DepthOnly
}
