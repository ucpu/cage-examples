#include <cage-core/files.h>
#include <cage-core/image.h>
#include <cage-core/imageAlgorithms.h>
#include <cage-core/meshAlgorithms.h>
#include <cage-core/meshExport.h>
#include <cage-core/meshShapes.h>

using namespace cage;

int main(int argc, const char *args[])
{
	const String basePath = pathJoin(pathSearchTowardsRoot("data", PathTypeFlags::Directory), "data/scenes/lods");
	const String scenesPath = pathJoin(pathSearchTowardsRoot("scenes-maps", PathTypeFlags::Directory), "lods.txt");

	Holder<File> fa = writeFile(pathJoin(basePath, "lods.assets"));
	fa->writeLine(Stringizer() + "[]");
	fa->writeLine(Stringizer() + "scheme = pack");
	fa->writeLine(Stringizer() + "lods.pack");
	fa->writeLine("");
	fa->writeLine(Stringizer() + "[]");
	fa->writeLine(Stringizer() + "scheme = object");
	fa->writeLine(Stringizer() + "lods.object");

	Holder<File> fo = writeFile(pathJoin(basePath, "lods.object"));
	fo->writeLine(Stringizer() + "[size]");
	fo->writeLine(Stringizer() + "world = 2");
	fo->writeLine(Stringizer() + "pixels = 200");

	{
		Holder<File> fs = writeFile(scenesPath);
		fs->writeLine("scenes/lods/lods.pack");
		fs->writeLine("");
		fs->writeLine("scenes/lods/lods.object");
		fs->writeLine("0 1 0");
		fs->writeLine("0");
		fs->writeLine("");
		fs->writeLine("scenes/common/ground.obj");
		fs->writeLine("0 0 0");
		fs->writeLine("0");
	}

	{
		Holder<File> fp = writeFile(pathJoin(basePath, "lods.pack"));
		fp->writeLine(Stringizer() + "[]");
		fp->writeLine(Stringizer() + "lods.object");
	}

	for (uint32 lod = 0; lod < 5; lod++)
	{
		Holder<Mesh> msh = newMeshSphereRegular(1, 1.0 / (1u << lod));
		meshMergeCloseVertices(+msh, {});
		meshRemoveInvalid(+msh);

		uint32 resolution = 0;
		{
			MeshUnwrapConfig cfg;
			cfg.texelsPerUnit = 100.0 * (1u << lod);
			resolution = meshUnwrap(+msh, cfg);
		}

		Holder<Image> texAlbedo = newImage();
		texAlbedo->initialize(Vec2i(resolution), 3);

		{
			MeshGenerateTextureConfig cfg;
			cfg.width = cfg.height = resolution;
			const std::pair ctx = { +msh, +texAlbedo };
			cfg.generator = [&ctx](const Vec2i &xy, const Vec3i &ids, const Vec3 &weights)
			{
				const Vec3 pos = ctx.first->positionAt(ids, weights);
				const Vec3 color = abs(pos);
				ctx.second->set(xy, color);
			};
			meshGenerateTexture(+msh, cfg);
		}

		imageDilation(+texAlbedo, 4);
		texAlbedo->exportFile(pathJoin(basePath, Stringizer() + "lod" + lod + ".png"));

		{
			MeshExportObjConfig cfg;
			cfg.mesh = +msh;
			meshExportFiles(pathJoin(basePath, Stringizer() + "lod" + lod + ".obj"), cfg);
		}

		{
			Holder<File> fc = writeFile(pathJoin(basePath, Stringizer() + "lod" + lod + ".obj_DefaultMaterial.cpm"));
			fc->writeLine(Stringizer() + "[base]");
			fc->writeLine(Stringizer() + "roughness = 0.7");
			fc->writeLine(Stringizer() + "metallic = 0");
			fc->writeLine(Stringizer() + "[textures]");
			fc->writeLine(Stringizer() + "albedo = " + "lod" + lod + ".png");
		}

		{
			fa->writeLine("");
			fa->writeLine(Stringizer() + "[]");
			fa->writeLine(Stringizer() + "scheme = texture");
			fa->writeLine(Stringizer() + "srgb = true");
			fa->writeLine(Stringizer() + "lod" + lod + ".png");
		}

		{
			fa->writeLine("");
			fa->writeLine(Stringizer() + "[]");
			fa->writeLine(Stringizer() + "scheme = model");
			fa->writeLine(Stringizer() + "lod" + lod + ".obj");
		}

		{
			fo->writeLine("");
			fo->writeLine(Stringizer() + "[]");
			fo->writeLine(Stringizer() + "threshold = " + (1u << lod));
			fo->writeLine(Stringizer() + "lod" + lod + ".obj");
		}
	}

	return 0;
}
