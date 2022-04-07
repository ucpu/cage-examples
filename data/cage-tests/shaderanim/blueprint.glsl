
$include /cage/shader/shaderConventions.h

$include /cage/shader/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shader/engine/fragment.glsl

void main()
{
	updateNormal();
	Material material = loadMaterial();
	float incident = smoothstep(0.6, 0.3, abs(dot(normal, normalize(uniViewport.eyePos.xyz - varPosition))));
	float posSum = varPosition.x + varPosition.y + varPosition.z;
	float wave = sin(posSum * 10 + uniViewport.time.z * 300 * 2 * 3.14159) * 0.5 + 0.5;
	material.albedo = mix(material.albedo, vec3(0, 0, 1), wave);
	material.opacity *= incident;
	outColor = lighting(material);
}
