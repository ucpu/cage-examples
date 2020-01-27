#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/assetStructs.h>
#include <cage-core/assetManager.h>
#include <cage-core/timer.h>
#include <cage-core/hashString.h>

#include <cage-engine/window.h>
#include <cage-engine/graphics.h>
#include <cage-engine/sound.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <atomic>

using namespace cage;

std::atomic<bool> destroying;
Holder<Window> window;
Holder<SoundContext> sound;
Holder<AssetManager> assets;

void glThread()
{
	window->makeCurrent();
	while (!destroying)
	{
		while (assets->processCustomThread(1));
		threadSleep(1000);
	}
	assets->unloadCustomThread(1);
	window->makeNotCurrent();
}

void slThread()
{
	while (!destroying)
	{
		while (assets->processCustomThread(2));
		threadSleep(1000);
	}
	assets->unloadCustomThread(2);
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
		window = newWindow();
		window->makeNotCurrent();
		sound = newSoundContext(SoundContextCreateConfig(), "cage");

		// asset schemes
		assets = newAssetManager(AssetManagerCreateConfig());
		assets->defineScheme<AssetPack>(AssetSchemeIndexPack, genAssetSchemePack());
		assets->defineScheme<ShaderProgram>(AssetSchemeIndexShaderProgram, genAssetSchemeShaderProgram(1, window.get()));
		assets->defineScheme<Texture>(AssetSchemeIndexTexture, genAssetSchemeTexture(1, window.get()));
		assets->defineScheme<Mesh>(AssetSchemeIndexMesh, genAssetSchemeMesh(1, window.get()));
		assets->defineScheme<Font>(AssetSchemeIndexFont, genAssetSchemeFont(1, window.get()));
		assets->defineScheme<SoundSource>(AssetSchemeIndexSoundSource, genAssetSchemeSoundSource(2, sound.get()));

		// threads
		Holder<Thread> thrGl = newThread(Delegate<void()>().bind<&glThread>(), "opengl");
		Holder<Thread> thrSl = newThread(Delegate<void()>().bind<&slThread>(), "sound");

		// asset names
		uint32 names[] = {
			HashString("cage/cage.pack"),
			HashString("cage/texture/texture.pack"),
			HashString("cage/sound/sound.pack"),
			HashString("cage/shader/shader.pack"),
			HashString("cage/mesh/mesh.pack"),
			HashString("cage/font/font.pack"),
			HashString("cage/cage.pack"),
			HashString("cage/texture/texture.pack"),
			HashString("cage/sound/sound.pack"),
			HashString("cage/shader/shader.pack"),
			HashString("cage/mesh/mesh.pack"),
			HashString("cage/font/font.pack"),
		};
		static const uint32 count = sizeof(names) / sizeof(names[0]);
		bool loaded[count];
		for (uint32 i = 0; i < count; i++)
			loaded[i] = false;

		CAGE_LOG(SeverityEnum::Info, "test", "starting the test, please wait");

		// loop
		for (uint32 step = 0; step < 30; step++)
		{
			uint64 stepEnd = getApplicationTime() + 1000 * 1000;
			while (getApplicationTime() < stepEnd)
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
			}
			CAGE_LOG(SeverityEnum::Info, "test", stringizer() + "step " + (step + 1) + "/30 finished");
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
		sound.clear();
		window.clear();

		CAGE_LOG(SeverityEnum::Info, "test", "done");
		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}

