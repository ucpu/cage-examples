#include <cage-core/assetContext.h>
#include <cage-core/assetManager.h>
#include <cage-core/concurrent.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>
#include <cage-engine/font.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/model.h>
#include <cage-engine/opengl.h>
#include <cage-engine/shaderProgram.h>
#include <cage-engine/sound.h>
#include <cage-engine/speaker.h>
#include <cage-engine/texture.h>
#include <cage-engine/voices.h>
#include <cage-engine/window.h>

using namespace cage;

bool closing = false;
constexpr uint32 assetsName = HashString("cage-tests/logo/logo.pack");

void windowClose(InputWindow)
{
	closing = true;
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
		Holder<Window> window = newWindow({});
		const auto closeListener = window->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));
		window->title("cage test logo");
		detail::initializeOpengl();

		// assets
		Holder<AssetManager> assets = newAssetManager(AssetManagerCreateConfig());
		assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
		assets->defineScheme<AssetSchemeIndexShaderProgram, MultiShaderProgram>(genAssetSchemeShaderProgram(0));
		assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(0));
		assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(0));
		assets->defineScheme<AssetSchemeIndexFont, Font>(genAssetSchemeFont(0));
		assets->defineScheme<AssetSchemeIndexSound, Sound>(genAssetSchemeSound());

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
			Holder<ShaderProgram> shader = assets->get<AssetSchemeIndexShaderProgram, MultiShaderProgram>(HashString("cage/shader/engine/blit.glsl"))->get(0);
			Holder<Sound> sound = assets->get<AssetSchemeIndexSound, Sound>(HashString("cage-tests/logo/logo.ogg"));

			// initialize graphics
			model->bind();
			texture->bind(0);
			shader->bind();

			// initialize sounds
			Holder<VoicesMixer> mixer = newVoicesMixer({});
			Holder<Speaker> speaker = newSpeaker({}, Delegate<void(const SoundCallbackData &)>().bind<VoicesMixer, &VoicesMixer::process>(+mixer));
			Holder<Voice> voice = mixer->newVoice();
			voice->sound = sound.share();
			speaker->start();

			// show the window
			Vec2i res(600, 600);
			window->windowedSize(res);
			window->setWindowed();

			// loop
			while (!closing)
			{
				res = window->resolution();
				glViewport(0, 0, res[0], res[1]);
				model->dispatch();
				speaker->process(applicationTime());
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
