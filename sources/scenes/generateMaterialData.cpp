#include <cage-core/files.h>
#include <cage-core/meshShapes.h>

using namespace cage;

constexpr uint32 RoughnessSteps = 15;
constexpr uint32 MetallicSteps = 12;

int main(int argc, const char *args[])
{
	const String basePath = pathJoin(pathSearchTowardsRoot("data", PathTypeFlags::Directory), "data/scenes/material");
	const String scenesPath = pathJoin(pathSearchTowardsRoot("scenes-maps", PathTypeFlags::Directory), "material.txt");

	Holder<File> fa = writeFile(pathJoin(basePath, "material.assets"));
	fa->writeLine(Stringizer() + "[]");
	fa->writeLine(Stringizer() + "scheme = pack");
	fa->writeLine(Stringizer() + "material.pack");
	fa->writeLine("");
	fa->writeLine(Stringizer() + "[]");
	fa->writeLine(Stringizer() + "scheme = model");
	fa->writeLine(Stringizer() + "uvs = false");

	Holder<File> fp = writeFile(pathJoin(basePath, "material.pack"));
	fp->writeLine(Stringizer() + "[]");

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
			const String mat = Stringizer() + "mat_" + r + "_" + m;

			{
				Holder<Mesh> msh = newMeshSphereRegular(1, 0.2);
				MeshExportObjConfig cfg;
				cfg.objectName = mat;
				msh->exportObjFile(pathJoin(basePath, mat + ".obj"), cfg);
			}

			{
				Holder<File> fc = writeFile(pathJoin(basePath, mat + ".obj_DefaultMaterial.cpm"));
				fc->writeLine(Stringizer() + "[base]");
				fc->writeLine(Stringizer() + "albedo = 0.95, 0.7, 0.2");
				fc->writeLine(Stringizer() + "roughness = " + ((r + 0.5) / RoughnessSteps));
				fc->writeLine(Stringizer() + "metallic = " + ((m + 0.5) / MetallicSteps));
			}

			fa->writeLine(Stringizer() + mat + ".obj");
			fp->writeLine(Stringizer() + mat + ".obj");

			fs->writeLine(Stringizer() + "scenes/material/" + mat + ".obj");
			fs->writeLine(Stringizer() + (sint32)(r * 3 - RoughnessSteps * 3 / 2) + " 0 " + (sint32)(m * 3 - MetallicSteps * 3 / 2));
			fs->writeLine(Stringizer() + "0");
			fs->writeLine("");
		}
	}

	return 0;
}
