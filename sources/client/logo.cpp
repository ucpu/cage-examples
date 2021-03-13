#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/assetContext.h>
#include <cage-core/assetManager.h>
#include <cage-core/timer.h>
#include <cage-core/hashString.h>

#include <cage-engine/window.h>
#include <cage-engine/graphics.h>
#include <cage-engine/sound.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/shaderConventions.h>

using namespace cage;

bool closing = false;
constexpr uint32 assetsName = HashString("cage-tests/logo/logo.pack");

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
		assets->defineScheme<ShaderProgram>(AssetSchemeIndexShaderProgram, genAssetSchemeShaderProgram(0));
		assets->defineScheme<Texture>(AssetSchemeIndexTexture, genAssetSchemeTexture(0));
		assets->defineScheme<Model>(AssetSchemeIndexModel, genAssetSchemeModel(0));
		assets->defineScheme<Font>(AssetSchemeIndexFont, genAssetSchemeFont(0));
		assets->defineScheme<SoundSource>(AssetSchemeIndexSoundSource, genAssetSchemeSoundSource(0));

		// load assets
		assets->add(assetsName);
		while (true)
		{
			if (assets->get<AssetSchemeIndexPack, AssetPack>(assetsName))
				break;
			assets->processCustomThread(0);
			threadSleep(1000);
		}

		{
			// fetch assets
			Holder<Model> model = assets->get<AssetSchemeIndexModel, Model>(HashString("cage/model/square.obj"));
			Holder<Texture> texture = assets->get<AssetSchemeIndexTexture, Texture>(HashString("cage-tests/logo/logo.png"));
			Holder<ShaderProgram> shader = assets->get<AssetSchemeIndexShaderProgram, ShaderProgram>(HashString("cage/shader/engine/blit.glsl"));
			Holder<SoundSource> source = assets->get<AssetSchemeIndexSoundSource, SoundSource>(HashString("cage-tests/logo/logo.ogg"));

			// initialize graphics
			model->bind();
			glActiveTexture(GL_TEXTURE0 + CAGE_SHADER_TEXTURE_COLOR);
			texture->bind();
			shader->bind();

			// initialize sounds
			Holder<Speaker> speaker = newSpeakerOutput(+sl, SpeakerCreateConfig(), "cage");
			Holder<MixingBus> bus = newMixingBus();
			speaker->setInput(+bus);
			source->addOutput(+bus);

			// show the window
			ivec2 res(600, 600);
			window->windowedSize(res);
			window->setWindowed();

			// loop
			while (!closing)
			{
				res = window->resolution();
				glViewport(0, 0, res[0], res[1]);
				model->dispatch();
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
		detail::logCurrentCaughtException();
		return 1;
	}
}
