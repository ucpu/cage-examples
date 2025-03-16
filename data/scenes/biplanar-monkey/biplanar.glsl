
$include /cage/shaders/shaderConventions.h

$include /cage/shaders/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shaders/engine/fragment.glsl

$include /cage/shaders/functions/simplex.glsl
$include /cage/shaders/functions/biplanar.glsl

float makeArrayIndex()
{
	float highest = -1000;
	float index = 0;
	for (int i = 0; i < 7; i++)
	{
		float s = simplex(vec4(varPosition * 0.15, i));
		if (s > highest)
		{
			highest = s;
			index = i;
		}
	}
	return index;
}

void main()
{
#ifndef DepthOnly
	float arrayIndex = makeArrayIndex();
	Material mat;
	normal = normalize(varNormal);
	Biplanar bip = biplanarPrepare(varPosition * 0.5, normal, 50);
	mat.albedo = biplanarSample(texMaterialAlbedoArray, bip, arrayIndex).rgb;
	vec4 spec = biplanarSample(texMaterialSpecialArray, bip, arrayIndex);
	mat.roughness = spec.x;
	mat.metalness = spec.y;
	mat.emissive = spec.z;
	mat.opacity = 1;
	mat.fade = 0;
	normal = biplanarSampleNormal(texMaterialNormalArray, bip, arrayIndex);
	mat3 nm = mat3(uniMeshes[varInstanceId].normalMat);
	normal = normalize(nm * normal);
	outColor = lighting(mat);
#endif // DepthOnly
}
