#include <cage-core/files.h>
#include <cage-core/meshShapes.h>

using namespace cage;

constexpr uint32 RoughnessSteps = 15;
constexpr uint32 MetallicSteps = 12;

int main(int argc, const char *args[])
{
	const string basePath = pathJoin(pathSearchTowardsRoot("data", PathTypeFlags::Directory), "data/scenes/material");
	const string scenesPath = pathJoin(pathSearchTowardsRoot("scenes-maps", PathTypeFlags::Directory), "material.txt");

	Holder<File> fa = writeFile(pathJoin(basePath, "material.assets"));
	fa->writeLine(stringizer() + "[]");
	fa->writeLine(stringizer() + "scheme = pack");
	fa->writeLine(stringizer() + "material.pack");
	fa->writeLine("");
	fa->writeLine(stringizer() + "[]");
	fa->writeLine(stringizer() + "scheme = model");
	fa->writeLine(stringizer() + "uvs = false");

	Holder<File> fp = writeFile(pathJoin(basePath, "material.pack"));
	fp->writeLine(stringizer() + "[]");

	Holder<File> fs = writeFile(scenesPath);
	fs->writeLine("scenes/material/material.pack");
	fs->writeLine("");
	fs->writeLine("scenes/common/ground.obj");
	fs->writeLine("0 0 0");
	fs->writeLine("0");
	fs->writeLine("");

	for (uint32 r = 0; r < RoughnessSteps; r++)
	{
		for (uint32 m = 0; m < MetallicSteps; m++)
		{
			const string mat = stringizer() + "mat_" + r + "_" + m;

			{
				Holder<Mesh> msh = newMeshSphereRegular(1, 0.2);
				MeshExportObjConfig cfg;
				cfg.objectName = mat;
				msh->exportObjFile(cfg, pathJoin(basePath, mat + ".obj"));
			}

			{
				Holder<File> fc = writeFile(pathJoin(basePath, mat + ".obj_DefaultMaterial.cpm"));
				fc->writeLine(stringizer() + "[base]");
				fc->writeLine(stringizer() + "albedo = 0.95, 0.7, 0.2");
				fc->writeLine(stringizer() + "roughness = " + ((r + 0.5) / RoughnessSteps));
				fc->writeLine(stringizer() + "metallic = " + ((m + 0.5) / MetallicSteps));
			}

			fa->writeLine(stringizer() + mat + ".obj");
			fp->writeLine(stringizer() + mat + ".obj");

			fs->writeLine(stringizer() + "scenes/material/" + mat + ".obj");
			fs->writeLine(stringizer() + (sint32)(r * 3 - RoughnessSteps * 3 / 2) + " 0 " + (sint32)(m * 3 - MetallicSteps * 3 / 2));
			fs->writeLine(stringizer() + "0");
			fs->writeLine("");
		}
	}

	return 0;
}
