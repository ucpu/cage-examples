#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/assetStructs.h>
#include <cage-core/assetManager.h>
#include <cage-core/timer.h>
#include <cage-core/hashString.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/graphics.h>
#include <cage-engine/sound.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/graphics/shaderConventions.h>

using namespace cage;

bool closing = false;
const uint32 assetsName = HashString("cage-tests/logo/logo.pack");

bool windowClose()
{
	closing = true;
	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		// window
		Holder<Window> window = newWindow();
		EventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		window->events.windowClose.attach(windowCloseListener);
		window->title("cage test logo");
		detail::initializeOpengl();

		// sound
		Holder<SoundContext> sl = newSoundContext(SoundContextCreateConfig(), "cage");

		// assets
		Holder<AssetManager> assets = newAssetManager(AssetManagerCreateConfig());
		assets->defineScheme<AssetPack>(AssetSchemeIndexPack, genAssetSchemePack());
		assets->defineScheme<ShaderProgram>(AssetSchemeIndexShaderProgram, genAssetSchemeShaderProgram(0, window.get()));
		assets->defineScheme<Texture>(AssetSchemeIndexTexture, genAssetSchemeTexture(0, window.get()));
		assets->defineScheme<Mesh>(AssetSchemeIndexMesh, genAssetSchemeMesh(0, window.get()));
		assets->defineScheme<Font>(AssetSchemeIndexFont, genAssetSchemeFont(0, window.get()));
		assets->defineScheme<SoundSource>(AssetSchemeIndexSoundSource, genAssetSchemeSoundSource(0, sl.get()));

		// load assets
		assets->add(assetsName);
		while (true)
		{
			if (assets->get<AssetSchemeIndexPack, AssetPack>(assetsName))
				break;
			assets->processCustomThread(0);
			threadSleep(1000);
		}

		// fetch assets
		Mesh *mesh = assets->get<AssetSchemeIndexMesh, Mesh>(HashString("cage/mesh/square.obj"));
		Texture *texture = assets->get<AssetSchemeIndexTexture, Texture>(HashString("cage-tests/logo/logo.png"));
		ShaderProgram *shader = assets->get<AssetSchemeIndexShaderProgram, ShaderProgram>(HashString("cage/shader/engine/blit.glsl"));
		SoundSource *source = assets->get<AssetSchemeIndexSoundSource, SoundSource>(HashString("cage-tests/logo/logo.ogg"));

		{
			// initialize graphics
			mesh->bind();
			glActiveTexture(GL_TEXTURE0 + CAGE_SHADER_TEXTURE_COLOR);
			texture->bind();
			shader->bind();

			// initialize sounds
			Holder<Speaker> speaker = newSpeakerOutput(sl.get(), SpeakerCreateConfig(), "cage");
			Holder<MixingBus> bus = newMixingBus(sl.get());
			speaker->setInput(bus.get());
			source->addOutput(bus.get());

			// show the window
			ivec2 res(600, 600);
			window->windowedSize(res);
			window->setWindowed();

			// loop
			while (!closing)
			{
				res = window->resolution();
				glViewport(0, 0, res.x, res.y);
				mesh->dispatch();
				speaker->update(getApplicationTime());
				threadSleep(10000);
				window->swapBuffers();
				window->processEvents();
			}
		}

		// unload assets
		assets->remove(assetsName);
		assets->unloadCustomThread(0);

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
