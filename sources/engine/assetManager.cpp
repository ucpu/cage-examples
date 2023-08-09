#include <atomic>

#include <cage-core/assetContext.h>
#include <cage-core/assetManager.h>
#include <cage-core/concurrent.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>
#include <cage-engine/font.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/model.h>
#include <cage-engine/shaderProgram.h>
#include <cage-engine/sound.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

std::atomic<bool> destroying;
Holder<Window> window;
Holder<AssetManager> assets;

// asset names
constexpr const uint32 names[] = {
	HashString("cage/cage.pack"),
	HashString("cage/texture/texture.pack"),
	HashString("cage/sound/sound.pack"),
	HashString("cage/shader/shader.pack"),
	HashString("cage/model/model.pack"),
	HashString("cage/font/font.pack"),
	HashString("cage/cage.pack"),
	HashString("cage/texture/texture.pack"),
	HashString("cage/sound/sound.pack"),
	HashString("cage/shader/shader.pack"),
	HashString("cage/model/model.pack"),
	HashString("cage/font/font.pack"),
};
constexpr uint32 count = sizeof(names) / sizeof(names[0]);

void glThread()
{
	window->makeCurrent();
	while (!destroying)
	{
		while (assets->processCustomThread(1))
			;
		threadSleep(1000);
		for (uint32 i = 0; i < 50; i++)
			assets->get<AssetSchemeIndexPack, AssetPack>(names[randomRange(0u, count)]);
	}
	assets->unloadCustomThread(1);
	window->makeNotCurrent();
}

void slThread()
{
	while (!destroying)
	{
		threadSleep(1000);
		for (uint32 i = 0; i < 50; i++)
			assets->get<AssetSchemeIndexPack, AssetPack>(names[randomRange(0u, count)]);
	}
}

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
		window->makeNotCurrent();

		// asset schemes
		AssetManagerCreateConfig cfg;
		assets = newAssetManager(cfg);
		assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
		assets->defineScheme<AssetSchemeIndexShaderProgram, MultiShaderProgram>(genAssetSchemeShaderProgram(1));
		assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(1));
		assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(1));
		assets->defineScheme<AssetSchemeIndexFont, Font>(genAssetSchemeFont(1));
		assets->defineScheme<AssetSchemeIndexSound, Sound>(genAssetSchemeSound());

		// threads
		Holder<Thread> thrGl = newThread(Delegate<void()>().bind<&glThread>(), "opengl");
		Holder<Thread> thrSl = newThread(Delegate<void()>().bind<&slThread>(), "sound");

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
						assets->remove(names[i]);
					else
						assets->add(names[i]);
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
				assets->remove(names[i]);
		assets->unloadWait();
		destroying = true;
		thrGl->wait();
		thrSl->wait();
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
