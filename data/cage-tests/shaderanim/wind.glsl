
$include /cage/shader/shaderConventions.h

$include /cage/shader/engine/vertex.glsl

float sqr(float x)
{
	return x * x;
}

vec3 applyWind(vec3 p, vec3 origin)
{
	float y = p.y;
	float k = length(vec2(p.x, p.z));
	float f = sqr(abs(y) * 0.1) * clamp(k, 0.5, 1.5);
	float x = f * sin(uniViewport.time.z * 520 + origin.x * 0.3 + 42);
	float z = f * sin(uniViewport.time.z * 480 + origin.z * 0.3 + 13);
	return p + vec3(x, 0, z);
}

void main()
{
	varInstanceId = gl_InstanceID;
	varPosition = inPosition;
	varNormal = inNormal;
	varTangent = inTangent;
	varUv = inUv;
	skeletalAnimation();
	vec3 origin = transpose(uniMeshes[varInstanceId].mMat) * vec4(0, 0, 0, 1);
	varPosition = applyWind(varPosition - origin, origin) + origin;
	gl_Position = uniMeshes[varInstanceId].mvpMat * vec4(varPosition, 1);
	varPosition = transpose(uniMeshes[varInstanceId].mMat) * vec4(varPosition, 1);
}

$include /cage/shader/engine/fragment.glsl

#ifndef CutOut
layout(early_fragment_tests) in;
#endif

void main()
{
#ifdef DepthOnly

#ifdef CutOut
	if (loadMaterial().opacity < 0.5)
		discard;
#endif // CutOut

#else // DepthOnly

	updateNormal();
	Material material = loadMaterial();
#ifdef CutOut
	if (material.opacity < 0.5)
		discard;
#endif // CutOut
	outColor = lighting(material);

#endif // DepthOnly
}
