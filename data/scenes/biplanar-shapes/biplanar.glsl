
$include /cage/shaders/shaderConventions.h

$include /cage/shaders/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shaders/engine/fragment.glsl

$include /cage/shaders/functions/simplex.glsl
$include /cage/shaders/functions/biplanar.glsl

void main()
{
#ifndef DepthOnly
	Material mat;
	normal = normalize(varNormal);
	mat.albedo = vec3(0, 0.2, 0.7);
	mat.roughness = 0.4;
	mat.metalness = 0;
	mat.emissive = 0;
	mat.opacity = 1;
	mat.fade = 0;
	Biplanar bip = biplanarPrepare(varPosition * 2, normal, 50);
	vec3 bent = biplanarSampleNormal(texMaterialNormal2d, bip);
	//normal = mix(normal, bent, sin(uniViewport.time.z * 3000) * 0.5 + 0.5);
	normal = bent;
	mat3 nm = transpose(mat3(uniMeshes[varInstanceId].modelMat));
	normal = normalize(nm * normal);
	outColor = lighting(mat);
#endif // DepthOnly
}
