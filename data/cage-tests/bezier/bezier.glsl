
$include /cage/shaders/shaderConventions.h

$include /cage/shaders/engine/vertex.glsl

vec3 bezierPoint(float t, vec3 points[4])
{
	float u = 1.0 - t;
    return u*u*u*points[0] + 3*u*u*t*points[1] + 3*u*t*t*points[2] + t*t*t*points[3];
}

vec3 bezierTangent(float t, vec3 points[4])
{
	float u = 1.0 - t;
	return 3*u*u*(points[1]-points[0]) + 6*u*t*(points[2]-points[1]) + 3*t*t*(points[3]-points[2]);
}

float bezierLength(float t, vec3 points[4])
{
	const float stepsize = 0.02;
	float i = stepsize;
	float sum = 0;
	vec3 p = points[0];
	while (i < t)
	{
		vec3 q = bezierPoint(i, points);
		sum += distance(p, q);
		p = q;
		i += stepsize;
	}
	return sum;
}

$define customDataCount 8
struct UniPoints
{
	vec4 a, b;
};
layout(std140, binding = CAGE_SHADER_UNIBLOCK_CUSTOMDATA) uniform CustomDataBlock
{
	UniPoints uniPoints[CAGE_SHADER_MAX_MESHES];
};

void main()
{
	varInstanceId = gl_InstanceID;
	mat4 mMat = mat4(transpose(uniMeshes[varInstanceId].mMat));
	vec3 points[4];
	points[0] = vec3(mMat * vec4(0, 0, 0, 1));
	points[1] = uniPoints[varInstanceId].a.xyz;
	points[2] = uniPoints[varInstanceId].b.xyz;
	points[3] = vec3(mMat * vec4(0, 0, -1, 1));
	varUv.x = inPosition[1] * 100; // side-by-side [-1..1]
	varUv.y = -inPosition[2]; // normalized length [0..1]
	varUv.z = bezierLength(-inPosition[2], points); // actual length [0..D]
	varPosition = bezierPoint(-inPosition[2], points);
	vec3 forward = bezierTangent(-inPosition[2], points);
	vec3 up = normalize(uniViewport.eyePos.xyz - varPosition);
	vec3 side = normalize(cross(forward, up));
	varPosition += side * inPosition[1] * 10;
	varPosition += up * 1;
	gl_Position = uniViewport.vpMat * vec4(varPosition, 1);
}

$include /cage/shaders/engine/fragment.glsl

void main()
{
#ifndef DepthOnly
	outColor = vec4(varUv.x * 0.5 + 0.5, varUv.y, 0, 1);
#endif // DepthOnly
}
