#include <atomic>

#include <cage-core/assetContext.h>
#include <cage-core/assetsManager.h>
#include <cage-core/assetsSchemes.h>
#include <cage-core/concurrent.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>
#include <cage-engine/assetsSchemes.h>
#include <cage-engine/font.h>
#include <cage-engine/graphicsDevice.h>
#include <cage-engine/model.h>
#include <cage-engine/shader.h>
#include <cage-engine/sound.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

std::atomic<bool> destroying;
Holder<Window> window;
Holder<AssetsManager> assets;

// asset names
constexpr const uint32 names[] = {
	HashString("cage/cage.pack"),
	HashString("cage/textures/textures.pack"),
	HashString("cage/sounds/sounds.pack"),
	HashString("cage/shaders/shaders.pack"),
	HashString("cage/models/models.pack"),
	HashString("cage/fonts/fonts.pack"),
	HashString("cage/cage.pack"),
	HashString("cage/textures/textures.pack"),
	HashString("cage/sounds/sounds.pack"),
	HashString("cage/shaders/shaders.pack"),
	HashString("cage/models/models.pack"),
	HashString("cage/fonts/fonts.pack"),
};
constexpr uint32 count = sizeof(names) / sizeof(names[0]);

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		// contexts
		window = newWindow({});
		Holder<GraphicsDevice> device = newGraphicsDevice({});

		// asset schemes
		AssetManagerCreateConfig cfg;
		assets = newAssetsManager(cfg);
		assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
		assets->defineScheme<AssetSchemeIndexShader, MultiShader>(genAssetSchemeShader(+device));
		assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(+device));
		assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(+device));
		assets->defineScheme<AssetSchemeIndexFont, Font>(genAssetSchemeFont());
		assets->defineScheme<AssetSchemeIndexSound, Sound>(genAssetSchemeSound());

		// assets
		bool loaded[count];
		for (uint32 i = 0; i < count; i++)
			loaded[i] = false;

		CAGE_LOG(SeverityEnum::Info, "test", "starting the test, please wait");

		// loop
		for (uint32 step = 0; step < 30; step++)
		{
			uint64 stepEnd = applicationTime() + 1000 * 1000;
			while (applicationTime() < stepEnd)
			{
				if (randomChance() < 0.7)
				{
					uint32 i = randomRange(0u, count);
					CAGE_ASSERT(i < count);
					if (loaded[i])
						assets->unload(names[i]);
					else
						assets->load(names[i]);
					loaded[i] = !loaded[i];
				}
				else
					threadSleep(1000);
				for (uint32 i = 0; i < 50; i++)
					assets->get<AssetSchemeIndexPack, AssetPack>(names[randomRange(0u, count)]);
			}
			CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "step " + (step + 1) + "/30 finished");
		}

		// clean up
		CAGE_LOG(SeverityEnum::Info, "test", "cleaning");
		for (uint32 i = 0; i < count; i++)
			if (loaded[i])
				assets->unload(names[i]);
		assets->waitTillEmpty();
		destroying = true;
		window.clear();

		CAGE_LOG(SeverityEnum::Info, "test", "done");
		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
