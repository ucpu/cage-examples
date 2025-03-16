
$include /cage/shaders/shaderConventions.h

$include /cage/shaders/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shaders/engine/fragment.glsl

$include /cage/shaders/functions/fbm.glsl

layout(early_fragment_tests) in;

float sampleHeight(vec3 wp)
{
	return fbm(wp + vec3(uniViewport.time.z * 100));
}

void main()
{
#ifndef DepthOnly

	vec3 pDx = dFdx(varPosition);
	vec3 pDy = dFdy(varPosition);
	float hll = sampleHeight(varPosition);
	float hlr = sampleHeight(varPosition + pDx);
	float hul = sampleHeight(varPosition + pDy);
	normal = normalize(varNormal);
	vec3 r1 = cross(pDy, normal);
	vec3 r2 = cross(normal, pDx);
	float det = dot(pDx, r1);
	vec3 surfGrad = sign(det) * ((hlr - hll) * r1 + (hul - hll) * r2);
	normal = normalize(abs(det) * normal - surfGrad);

	Material material = loadMaterial();
	outColor = lighting(material);

#endif // DepthOnly
}
