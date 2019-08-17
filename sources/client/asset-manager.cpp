#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/assetStructs.h>
#include <cage-core/assetManager.h>
#include <cage-core/timer.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/graphics.h>
#include <cage-client/sound.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

volatile bool destroying;
holder<windowHandle> window;
holder<soundContext> sound;
holder<assetManager> assets;

void glThread()
{
	window->makeCurrent();
	while (!destroying)
	{
		while (assets->processCustomThread(1));
		threadSleep(5000);
	}
	window->makeNotCurrent();
}

void slThread()
{
	while (!destroying)
	{
		while (assets->processCustomThread(2));
		threadSleep(5000);
	}
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		// contexts
		window = newWindow();
		window->makeNotCurrent();
		sound = newSoundContext(soundContextCreateConfig(), "cage");

		// asset schemes
		assets = newAssetManager(assetManagerCreateConfig());
		assets->defineScheme<void>(assetSchemeIndexPack, genAssetSchemePack(0));
		assets->defineScheme<shaderProgram>(assetSchemeIndexShaderProgram, genAssetSchemeShaderProgram(1, window.get()));
		assets->defineScheme<renderTexture>(assetSchemeIndexRenderTexture, genAssetSchemeRenderTexture(1, window.get()));
		assets->defineScheme<renderMesh>(assetSchemeIndexMesh, genAssetSchemeRenderMesh(1, window.get()));
		assets->defineScheme<fontFace>(assetSchemeIndexFontFace, genAssetSchemeFontFace(1, window.get()));
		assets->defineScheme<soundSource>(assetSchemeIndexSoundSource, genAssetSchemeSoundSource(2, sound.get()));

		// threads
		holder<threadHandle> thrGl = newThread(delegate<void()>().bind<&glThread>(), "opengl");
		holder<threadHandle> thrSl = newThread(delegate<void()>().bind<&slThread>(), "sound");

		// asset names
		uint32 names[] = {
			hashString("cage/cage.pack"),
			hashString("cage/texture/texture.pack"),
			hashString("cage/sound/sound.pack"),
			hashString("cage/shader/shader.pack"),
			hashString("cage/mesh/mesh.pack"),
			hashString("cage/font/font.pack"),
			hashString("cage/cage.pack"),
			hashString("cage/texture/texture.pack"),
			hashString("cage/sound/sound.pack"),
			hashString("cage/shader/shader.pack"),
			hashString("cage/mesh/mesh.pack"),
			hashString("cage/font/font.pack"),
			42,
		};
		static const uint32 count = sizeof(names) / sizeof(names[0]);
		bool loaded[count];
		for (uint32 i = 0; i < count; i++)
			loaded[i] = false;

		assets->fabricate(assetSchemeIndexRenderTexture, 42, "fabricated texture");

		CAGE_LOG(severityEnum::Info, "test", "starting the test, please wait");

		// loop
		for (uint32 step = 0; step < 30; step++)
		{
			uint64 stepEnd = getApplicationTime() + 1000 * 1000;
			while (getApplicationTime() < stepEnd)
			{
				if (randomChance() < 0.7)
				{
					uint32 i = randomRange((uint32)0, count);
					CAGE_ASSERT(i < count);
					if (loaded[i])
						assets->remove(names[i]);
					else
						assets->add(names[i]);
					loaded[i] = !loaded[i];
				}
				else
				{
					while (assets->processControlThread() || assets->processCustomThread(0));
					threadSleep(1000);
				}
			}
			CAGE_LOG(severityEnum::Info, "test", string() + "step " + (step + 1) + "/30 finished");
		}

		CAGE_LOG(severityEnum::Info, "test", "test finished");

		// clean up
		assets->remove(42);
		for (uint32 i = 0; i < count; i++)
			if (loaded[i])
				assets->remove(names[i]);
		while (assets->countTotal())
		{
			while (assets->processControlThread() || assets->processCustomThread(0));
			threadSleep(5000);
		}
		destroying = true;
		thrGl->wait();
		thrSl->wait();
		sound.clear();
		window.clear();
		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}

