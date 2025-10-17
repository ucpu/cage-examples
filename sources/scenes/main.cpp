#include <cage-core/assetContext.h>
#include <cage-core/assetsManager.h>
#include <cage-core/assetsOnDemand.h>
#include <cage-core/assetsSchemes.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>
#include <cage-engine/assetsSchemes.h>
#include <cage-engine/font.h>
#include <cage-engine/graphicsDevice.h>
#include <cage-engine/graphicsEncoder.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/inputs.h>
#include <cage-engine/model.h>
#include <cage-engine/renderObject.h>
#include <cage-engine/shader.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

int main(int argc, const char *args[])
{
	initializeConsoleLogger();

	Holder<GraphicsDevice> device = newGraphicsDevice({});
	Holder<Window> window = newWindow({});

	Holder<AssetsManager> assets = newAssetsManager({});
	Holder<AssetsOnDemand> onDemand = newAssetsOnDemand(+assets);
	assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
	assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(+device));
	assets->defineScheme<AssetSchemeIndexShader, MultiShader>(genAssetSchemeShader(+device));
	assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(+device));
	assets->defineScheme<AssetSchemeIndexRenderObject, RenderObject>(genAssetSchemeRenderObject());
	assets->defineScheme<AssetSchemeIndexFont, Font>(genAssetSchemeFont());

	assets->load(HashString("cage/cage.pack"));
	assets->load(HashString("scenes/mcguire/crytek/sponza-preload.object"));
	assets->load(HashString("scenes/mcguire/crytek/unlit.glsl"));

	while (assets->processing())
	{
		assets->processCustomThread(0);
		window->processEvents();
		device->processEvents();
	}

	Holder<Shader> shader = assets->get<MultiShader>(HashString("scenes/mcguire/crytek/unlit.glsl"))->get(0);
	CAGE_ASSERT(shader);

	Holder<Font> font = assets->get<Font>(HashString("cage/fonts/ubuntu/regular.ttf"));
	CAGE_ASSERT(font);

	GuiManagerCreateConfig guicfg;
	guicfg.assetManager = +assets;
	guicfg.graphicsDevice = +device;
	Holder<GuiManager> gui = newGuiManager(guicfg);

	{
		Holder<GuiBuilder> g = newGuiBuilder(gui->entities());
		auto _1 = g->alignment(Vec2(0));
		auto _2 = g->panel().text("Whatsup");
		auto _3 = g->column();
		g->label().text("rambo");
		g->label().text("sneaky");
		g->label().text("violin");
	}

	window->windowedSize(Vec2i(1700, 1000));
	window->setWindowed();

	bool closing = false;
	const auto eventClose = window->events.listen(inputFilter([&](input::WindowClose) { closing = true; }));
	const auto eventGui = window->events.listen([&](GenericInput in) { gui->handleInput(in); });

	Holder<Timer> timer = newTimer();
	uint32 framesCount = 0;

	while (!closing)
	{
		device->processEvents();
		Holder<Texture> surfaceTexture = device->nextFrame(+window);
		if (!surfaceTexture)
		{
			timer->reset();
			framesCount = 0;
			continue;
		}

		if (++framesCount > 1000)
		{
			CAGE_LOG(SeverityEnum::Info, "timer", Stringizer() + timer->elapsed());
			framesCount = 0;
		}

		{
			Holder<Texture> depthTexture = newTexture(+device, DepthTextureCreateConfig{ surfaceTexture->resolution() });

			Holder<GraphicsEncoder> encoder = newGraphicsEncoder(+device);
			encoder->setLabel("sponza");
			encoder->nextPass(PassConfig{ { PassConfig::ColorTargetConfig{ +surfaceTexture, Vec4(0.1, 0.1, 0.1, 1) } }, PassConfig::DepthTargetConfig{ +depthTexture } });

			for (uint32 i = 0; i <= 24; i++)
			{
				Holder<Model> model = assets->get<Model>(HashString(String(Stringizer() + "scenes/mcguire/crytek/converted/sponza_" + i + ".glb")));
				prepareModelBindings(+device, +assets, +model);
				DrawConfig draw;
				draw.model = +model;
				draw.shader = +shader;
				encoder->draw(draw);
			}

			{
				const auto text = font->layout("Hello there", {});
				FontRenderConfig cfg;
				cfg.assets = +onDemand;
				cfg.queue = +encoder;
				cfg.transform = Mat4(Transform(Vec3(), Quat(), 0.01));
				font->render(text, cfg);
			}

			encoder->submit();
		}

		{
			Holder<GraphicsEncoder> encoder = newGraphicsEncoder(+device);
			encoder->setLabel("gui");
			encoder->nextPass(PassConfig{ { PassConfig::ColorTargetConfig{ +surfaceTexture, Vec4(), false } } });
			gui->outputResolution(window->resolution());
			gui->outputRetina(window->contentScaling());
			gui->prepare();
			window->processEvents();
			gui->finish(+encoder);
			encoder->submit();
		}
	}

	assets->unload(HashString("cage/cage.pack"));
	assets->unload(HashString("scenes/mcguire/crytek/sponza-preload.object"));
	assets->unload(HashString("scenes/mcguire/crytek/unlit.glsl"));

	while (assets->processing())
	{
		assets->processCustomThread(0);
		device->processEvents();
	}

	return 0;
}
