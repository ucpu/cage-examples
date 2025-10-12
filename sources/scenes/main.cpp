#include <webgpu/webgpu_cpp.h>

#include <cage-core/image.h>
#include <cage-core/logger.h>
#include <cage-core/meshAlgorithms.h>
#include <cage-core/meshShapes.h>
#include <cage-engine/gpuBuffer.h>
#include <cage-engine/graphicsDevice.h>
#include <cage-engine/graphicsEncoder.h>
#include <cage-engine/model.h>
#include <cage-engine/shader.h>
#include <cage-engine/spirv.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

int main(int argc, const char *args[])
{
	initializeConsoleLogger();

	Holder<Window> window = newWindow({});
	Holder<GraphicsDevice> device = newGraphicsDevice({});

	Holder<Spirv> spirv = newSpirv();
	{
		static constexpr const char *vertex = R"glsl(
#version 450 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 0) out vec2 varUv;
void main()
{
	gl_Position = vec4(inPosition, 1);
	varUv=inUv;
}
)glsl";
		static constexpr const char *fragment = R"glsl(
#version 450 core
layout(location = 0) in vec2 varUv;
layout(location = 0) out vec4 color;
layout(set = 0, binding = 0) uniform sampler2D uniTex;
void main()
{
	color = texture(uniTex, varUv);
}
)glsl";
		spirv->importGlsl({ vertex, fragment });
	}
	Holder<Shader> shader = newShader(+device, +spirv);

	Holder<Mesh> mesh = newMeshIcosahedron(0.5);
	meshConvertToExpanded(+mesh);
	{
		std::vector<Vec2> uv;
		uv.reserve(mesh->verticesCount());
		for (Vec3 v : mesh->positions())
			uv.push_back(Vec2(v));
		mesh->uvs(uv);
	}
	Holder<Model> model = newModel(+device, +mesh, {});

	Holder<Image> image = newImage();
	image->initialize(Vec2i(512, 512));
	for (uint32 y = 0; y < 512; y++)
		for (uint32 x = 0; x < 512; x++)
			image->set(Vec2i(x, y), Vec4(x / 512.0, y / 512.0, 0, 1));
	Holder<Texture> colorTexture = newTexture(+device, +image);

	window->setWindowed();

	while (true)
	{
		window->processEvents();
		device->processEvents();
		Holder<Texture> surfaceTexture = device->nextFrame(+window);
		if (!surfaceTexture)
			continue;

		Holder<GraphicsEncoder> encoder = newGraphicsEncoder(+device);
		encoder->nextPass(PassConfig{ { PassConfig::TargetConfig{ +surfaceTexture, Vec4(0.1, 0.1, 0.1, 1), true } } });
		{
			DrawConfig draw;
			draw.materialTextures[0] = +colorTexture;
			draw.model = +model;
			draw.shader = +shader;
			encoder->draw(draw);
		}
		encoder->submit();
	}

	return 0;
}
