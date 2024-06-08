
$include /cage/shader/shaderConventions.h

$include /cage/shader/engine/vertex.glsl

void main()
{
	updateVertex();
}

$include /cage/shader/engine/fragment.glsl
$include simplex.glsl

struct Biplanar
{
	vec2 uv1, uv2;
	vec2 d1x, d2x;
	vec2 d1y, d2y;
	vec2 w;
	float div;
};

Biplanar biplanarPrepare(vec3 p, vec3 n, float k)
{
	Biplanar bip;
	vec3 dpdx = dFdx(p);
	vec3 dpdy = dFdy(p);
	n = abs(n);
	ivec3 ma = (n.x > n.y && n.x > n.z) ? ivec3(0, 1, 2) : (n.y > n.z) ? ivec3(1, 2, 0) : ivec3(2, 0, 1);
	ivec3 mi = (n.x < n.y && n.x < n.z) ? ivec3(0, 1, 2) : (n.y < n.z) ? ivec3(1, 2, 0) : ivec3(2, 0, 1);
	ivec3 me = ivec3(3) - mi - ma;
	bip.uv1 = vec2(p[ma.y], p[ma.z]);
	bip.uv2 = vec2(p[me.y], p[me.z]);
	bip.d1x = vec2(dpdx[ma.y], dpdx[ma.z]);
	bip.d2x = vec2(dpdx[me.y], dpdx[me.z]);
	bip.d1y = vec2(dpdy[ma.y], dpdy[ma.z]);
	bip.d2y = vec2(dpdy[me.y], dpdy[me.z]);
	bip.w = vec2(n[ma.x], n[me.x]);
	bip.w = clamp((bip.w - 0.5773) / (1.0 - 0.5773), 0.0, 1.0);
	bip.w = pow(bip.w, vec2(k / 8.0));
	bip.div = 1 / (bip.w.x + bip.w.y);
	return bip;
}

vec4 biplanarSample(sampler2D sam, Biplanar bip)
{
	vec4 x = textureGrad(sam, bip.uv1, bip.d1x, bip.d1y);
	vec4 y = textureGrad(sam, bip.uv2, bip.d2x, bip.d2y);
	return (x * bip.w.x + y * bip.w.y) * bip.div;
}

vec4 biplanarSampleArray(sampler2DArray sam, Biplanar bip, float arrayIndex)
{
	vec4 x = textureGrad(sam, vec3(bip.uv1, arrayIndex), bip.d1x, bip.d1y);
	vec4 y = textureGrad(sam, vec3(bip.uv2, arrayIndex), bip.d2x, bip.d2y);
	return (x * bip.w.x + y * bip.w.y) * bip.div;
}

vec3 biplanarSampleNormal(sampler2D sam, Biplanar bip, vec3 normal)
{
	vec3 n1 = restoreNormalMap(textureGrad(sam, bip.uv1, bip.d1x, bip.d1y));
	vec3 n2 = restoreNormalMap(textureGrad(sam, bip.uv2, bip.d2x, bip.d2y));
	mat3 tbn = makeTangentSpace(normal, varPosition - uniViewport.eyePos.xyz, bip.uv1);
	n1 = tbn * n1;
	tbn = makeTangentSpace(normal, varPosition - uniViewport.eyePos.xyz, bip.uv2);
	n2 = tbn * n2;
	return normalize(n1 * bip.w.x + n2 * bip.w.y);
}

vec3 biplanarSampleNormalArray(sampler2DArray sam, Biplanar bip, vec3 normal, float arrayIndex)
{
	vec3 n1 = restoreNormalMap(textureGrad(sam, vec3(bip.uv1, arrayIndex), bip.d1x, bip.d1y));
	vec3 n2 = restoreNormalMap(textureGrad(sam, vec3(bip.uv2, arrayIndex), bip.d2x, bip.d2y));
	mat3 tbn = makeTangentSpace(normal, varPosition - uniViewport.eyePos.xyz, bip.uv1);
	n1 = tbn * n1;
	tbn = makeTangentSpace(normal, varPosition - uniViewport.eyePos.xyz, bip.uv2);
	n2 = tbn * n2;
	return normalize(n1 * bip.w.x + n2 * bip.w.y);
}

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
	mat3 nm = mat3(uniMeshes[varInstanceId].normalMat);
	normal = normalize(nm * normal);
	Biplanar bip = biplanarPrepare(varPosition * 0.2, normal, 50);
	mat.albedo = biplanarSampleArray(texMaterialAlbedoArray, bip, arrayIndex).rgb;
	vec4 spec = biplanarSampleArray(texMaterialSpecialArray, bip, arrayIndex);
	mat.roughness = spec.x;
	mat.metalness = spec.y;
	mat.emissive = spec.z;
	mat.opacity = 1;
	mat.fade = 0;
	normal = biplanarSampleNormalArray(texMaterialNormalArray, bip, normalize(varNormal), arrayIndex);
	normal = normalize(nm * normal);
	outColor = lighting(mat);
#endif // DepthOnly
}
