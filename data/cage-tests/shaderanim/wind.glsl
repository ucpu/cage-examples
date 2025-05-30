
$include /cage/shaders/shaderConventions.h

$include /cage/shaders/engine/vertex.glsl

vec3 applyWind(vec3 p, vec3 origin)
{
	float k = length(vec2(p.x, p.z));
	float f = sqr(abs(p.y) * 0.08) * clamp(k, 0.5, 1.5);
	const float norm = 1 / length(vec3(17, 19, 23)) * 0.02;
	float x = f * sin(uniViewport.time.z * 520 + dot(origin, vec3(17, 19, 23)) * norm + 42);
	float z = f * sin(uniViewport.time.z * 480 + dot(origin, vec3(23, 17, 19)) * norm + 13);
	return p + vec3(x, 0, z);
}

void main()
{
	varInstanceId = gl_InstanceID;
	varPosition = inPosition;
	varNormal = inNormal;
	varUv = inUv;
	//skeletalAnimation();
	vec3 origin = transpose(uniMeshes[varInstanceId].mMat) * vec4(0, 0, 0, 1);
	varPosition = applyWind(varPosition, origin);
	gl_Position = uniMeshes[varInstanceId].mvpMat * vec4(varPosition, 1);
	varPosition = transpose(uniMeshes[varInstanceId].mMat) * vec4(varPosition, 1);
}

$include /cage/shaders/engine/fragment.glsl

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
