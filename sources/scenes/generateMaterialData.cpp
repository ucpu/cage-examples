#include <cage-core/core.h>
#include <cage-core/files.h>
#include <cage-core/fileUtils.h>
#include <cage-core/math.h>

using namespace cage;

namespace
{
	static const vec3 sphereCoords[] = {
		vec3(0.000000, -1.000000, 0.000000),
		vec3(0.723607, -0.447220, 0.525725),
		vec3(-0.276388, -0.447220, 0.850649),
		vec3(-0.894426, -0.447216, 0.000000),
		vec3(-0.276388, -0.447220, -0.850649),
		vec3(0.723607, -0.447220, -0.525725),
		vec3(0.276388, 0.447220, 0.850649),
		vec3(-0.723607, 0.447220, 0.525725),
		vec3(-0.723607, 0.447220, -0.525725),
		vec3(0.276388, 0.447220, -0.850649),
		vec3(0.894426, 0.447216, 0.000000),
		vec3(0.000000, 1.000000, 0.000000),
		vec3(-0.162456, -0.850654, 0.499995),
		vec3(0.425323, -0.850654, 0.309011),
		vec3(0.262869, -0.525738, 0.809012),
		vec3(0.850648, -0.525736, 0.000000),
		vec3(0.425323, -0.850654, -0.309011),
		vec3(-0.525730, -0.850652, 0.000000),
		vec3(-0.688189, -0.525736, 0.499997),
		vec3(-0.162456, -0.850654, -0.499995),
		vec3(-0.688189, -0.525736, -0.499997),
		vec3(0.262869, -0.525738, -0.809012),
		vec3(0.951058, 0.000000, 0.309013),
		vec3(0.951058, 0.000000, -0.309013),
		vec3(0.000000, 0.000000, 1.000000),
		vec3(0.587786, 0.000000, 0.809017),
		vec3(-0.951058, 0.000000, 0.309013),
		vec3(-0.587786, 0.000000, 0.809017),
		vec3(-0.587786, 0.000000, -0.809017),
		vec3(-0.951058, 0.000000, -0.309013),
		vec3(0.587786, 0.000000, -0.809017),
		vec3(0.000000, 0.000000, -1.000000),
		vec3(0.688189, 0.525736, 0.499997),
		vec3(-0.262869, 0.525738, 0.809012),
		vec3(-0.850648, 0.525736, 0.000000),
		vec3(-0.262869, 0.525738, -0.809012),
		vec3(0.688189, 0.525736, -0.499997),
		vec3(0.162456, 0.850654, 0.499995),
		vec3(0.525730, 0.850652, 0.000000),
		vec3(-0.425323, 0.850654, 0.309011),
		vec3(-0.425323, 0.850654, -0.309011),
		vec3(0.162456, 0.850654, -0.499995),
	};
	static const uint32 coordsCount = sizeof(sphereCoords) / sizeof(sphereCoords[0]);

	static const uint32 sphereIndexes[] = {
		1, 14, 13,
		2, 14, 16,
		1, 13, 18,
		1, 18, 20,
		1, 20, 17,
		2, 16, 23,
		3, 15, 25,
		4, 19, 27,
		5, 21, 29,
		6, 22, 31,
		2, 23, 26,
		3, 25, 28,
		4, 27, 30,
		5, 29, 32,
		6, 31, 24,
		7, 33, 38,
		8, 34, 40,
		9, 35, 41,
		10, 36, 42,
		11, 37, 39,
		39, 42, 12,
		39, 37, 42,
		37, 10, 42,
		42, 41, 12,
		42, 36, 41,
		36, 9 , 41,
		41, 40, 12,
		41, 35, 40,
		35, 8 , 40,
		40, 38, 12,
		40, 34, 38,
		34, 7 , 38,
		38, 39, 12,
		38, 33, 39,
		33, 11, 39,
		24, 37, 11,
		24, 31, 37,
		31, 10, 37,
		32, 36, 10,
		32, 29, 36,
		29, 9 , 36,
		30, 35, 9 ,
		30, 27, 35,
		27, 8 , 35,
		28, 34, 8 ,
		28, 25, 34,
		25, 7 , 34,
		26, 33, 7 ,
		26, 23, 33,
		23, 11, 33,
		31, 32, 10,
		31, 22, 32,
		22, 5 , 32,
		29, 30, 9 ,
		29, 21, 30,
		21, 4 , 30,
		27, 28, 8 ,
		27, 19, 28,
		19, 3 , 28,
		25, 26, 7 ,
		25, 15, 26,
		15, 2 , 26,
		23, 24, 11,
		23, 16, 24,
		16, 6 , 24,
		17, 22, 6 ,
		17, 20, 22,
		20, 5 , 22,
		20, 21, 5 ,
		20, 18, 21,
		18, 4 , 21,
		18, 19, 4 ,
		18, 13, 19,
		13, 3 , 19,
		16, 17, 6 ,
		16, 14, 17,
		14, 1 , 17,
		13, 15, 3 ,
		13, 14, 15,
		14, 2 , 15,
	};
	static const uint32 facesCount = sizeof(sphereIndexes) / sizeof(sphereIndexes[0]) / 3;
}

static const uint32 R = 15;
static const uint32 M = 12;

int main(int argc, const char *args[])
{
	string basePath = pathJoin(pathSearchTowardsRoot("data", pathTypeFlags::Directory), "scenes/material");
	string scenesPath = pathJoin(pathSearchTowardsRoot("scenes-maps", pathTypeFlags::Directory), "material.txt");
	holder<fileHandle> fo = newFile(pathJoin(basePath, "material.obj"), fileMode(false, true));
	{
		fo->writeLine("mtllib material.mtl");
		for (uint32 i = 0; i < coordsCount; i++)
		{
			fo->writeLine(string() + "v  " + sphereCoords[i][0] + " " + sphereCoords[i][1] + " " + sphereCoords[i][2]);
			fo->writeLine(string() + "vn " + sphereCoords[i][0] + " " + sphereCoords[i][1] + " " + sphereCoords[i][2]);
		}
	}
	holder<fileHandle> fm = newFile(pathJoin(basePath, "material.mtl"), fileMode(false, true));
	holder<fileHandle> fa = newFile(pathJoin(basePath, "material.asset"), fileMode(false, true));
	{
		fa->writeLine(string() + "[]");
		fa->writeLine(string() + "scheme = pack");
		fa->writeLine(string() + "material.pack");
		fa->writeLine(string() + "[]");
		fa->writeLine(string() + "scheme = mesh");
		fa->writeLine(string() + "uvs = false");
	}
	holder<fileHandle> fp = newFile(pathJoin(basePath, "material.pack"), fileMode(false, true));
	{
		fp->writeLine(string() + "[]");
	}
	holder<fileHandle> fs = newFile(scenesPath, fileMode(false, true));
	fs->writeLine("scenes/material/material.pack");
	fs->writeLine("");
	fs->writeLine("scenes/common/ground.obj");
	fs->writeLine("0 0 0");
	fs->writeLine("0");
	for (uint32 r = 0; r < R; r++)
	{
		for (uint32 m = 0; m < M; m++)
		{
			string mat = string() + "mat_" + r + "_" + m;
			fm->writeLine(string() + "newmtl " + mat);
			fm->writeLine(string() + "Ns " + (r * M + m)); // this is to prevent assimp from merging the materials
			holder<fileHandle> fc = newFile(pathJoin(basePath, string() + "material.obj_" + mat + ".cpm"), fileMode(false, true));
			fc->writeLine(string() + "[base]");
			fc->writeLine(string() + "albedo = 0.95, 0.7, 0.2");
			fc->writeLine(string() + "roughness = " + ((r + 0.5) / R));
			fc->writeLine(string() + "metallic = " + ((m + 0.5) / M));
			fo->writeLine(string() + "o " + mat);
			fo->writeLine(string() + "usemtl " + mat);
			for (uint32 i = 0; i < facesCount; i++)
			{
				const uint32 *n = &sphereIndexes[i * 3];
				fo->writeLine(string() + "f " + n[0] + "//" + n[0] + " " + n[1] + "//" + n[1] + " " + n[2] + "//" + n[2]);
			}
			fa->writeLine(string() + "material.obj?" + mat);
			fp->writeLine(string() + "material.obj?" + mat);
			fs->writeLine(string() + "scenes/material/material.obj?" + mat);
			fs->writeLine(string() + (sint32)(r * 3 - R * 3 / 2) + " 0 " + (sint32)(m * 3 - M * 3 / 2));
			fs->writeLine(string() + "0");
			fs->writeLine("");
		}
	}
	return 0;
}
