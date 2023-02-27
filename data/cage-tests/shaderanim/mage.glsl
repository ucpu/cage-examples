
$include /cage/shader/shaderConventions.h

$include /cage/shader/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shader/engine/fragment.glsl

layout(early_fragment_tests) in;

#define NUM_NOISE_OCTAVES 5

float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
float hash(vec2 p) {vec3 p3 = fract(vec3(p.xyx) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

float noise(float x)
{
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x)
{
    vec2 i = floor(x);
    vec2 f = fract(x);
	float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float noise(vec3 x)
{
    const vec3 step = vec3(110, 241, 171);
    vec3 i = floor(x);
    vec3 f = fract(x);
    float n = dot(i, step);
    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

float fbm(float x)
{
	float v = 0.0;
	float a = 0.5;
	float shift = float(100);
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i)
	{
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


float fbm(vec2 x)
{
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
    const mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i)
	{
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


float fbm(vec3 x)
{
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i)
	{
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

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
