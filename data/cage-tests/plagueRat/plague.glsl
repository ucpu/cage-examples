
$include /cage/shaders/engine/vertex.glsl

void main()
{
	propagateInputs();
	skeletalAnimation();
	computePosition();
}

$include /cage/shaders/engine/fragment.glsl

#if !defined(MaterialTexRegular)
#error "unintended combination of keywords"
#endif

layout(set = 1, binding = 7) uniform sampler2D texPostules;


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
