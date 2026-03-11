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
#include <cage-engine/graphicsEncoder.h>
#include <cage-engine/model.h>
#include <cage-engine/shader.h>
#include <cage-engine/sound.h>
#include <cage-engine/soundsVoices.h>
#include <cage-engine/speaker.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

bool closing = false;
constexpr uint32 AssetsName = HashString("cage-tests/logo/logo.pack");

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();

		// window
		Holder<Window> window = newWindow({});
		const auto closeListener = window->events.listen(inputFilter([](input::WindowClose) { closing = true; }));
		window->title("cage test raw window");

		// graphics device
		Holder<GraphicsDevice> device = newGraphicsDevice({ +window });

		// speaker
		Holder<VoicesMixer> mixer = newVoicesMixer();
		Holder<Speaker> speaker = newSpeaker({ .callback = Delegate<void(const SoundCallbackData &)>().bind<VoicesMixer, &VoicesMixer::process>(+mixer) });

		// assets
		Holder<AssetsManager> assets = newAssetsManager(AssetsManagerCreateConfig());
		assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
		assets->defineScheme<AssetSchemeIndexShader, MultiShader>(genAssetSchemeShader(+device));
		assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(+device));
		assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(+device));
		assets->defineScheme<AssetSchemeIndexFont, Font>(genAssetSchemeFont());
		assets->defineScheme<AssetSchemeIndexSound, Sound>(genAssetSchemeSound());

		// load assets
		assets->load(AssetsName);
		while (true)
		{
			if (assets->get<AssetSchemeIndexPack, AssetPack>(AssetsName))
				break;
			assets->processCustomThread(0);
			threadSleep(1'000);
		}

		{
			// fetch assets
			Holder<Model> model = assets->get<AssetSchemeIndexModel, Model>(HashString("cage/models/square.obj"));
			Holder<Texture> texture = assets->get<AssetSchemeIndexTexture, Texture>(HashString("cage-tests/logo/logo.png"));
			Holder<Shader> shader = assets->get<AssetSchemeIndexShader, MultiShader>(HashString("cage/shaders/engine/blitPixels.glsl"))->get(0);
			Holder<Sound> sound = assets->get<AssetSchemeIndexSound, Sound>(HashString("cage-tests/logo/logo.ogg"));

			// sound
			Holder<Voice> voice = mixer->newVoice();
			voice->sound = sound.share();
			voice->loop = true;
			speaker->start();

			// show the window
			Vec2i res(600, 600);
			window->windowedSize(res);
			window->setWindowed();

			// loop
			while (!closing)
			{
				const auto frame = device->nextFrame(+window);
				if (frame.targetTexture)
				{
					res = frame.targetTexture->resolution();
					Holder<GraphicsEncoder> enc = newGraphicsEncoder(+device, "enc");
					RenderPassConfig pass;
					pass.colorTargets.push_back({ +frame.targetTexture });
					pass.colorTargets[0].clearValue = Vec4(randomChance3(), 1);
					enc->nextPass(pass);
					enc->scissors({}, res);
					/*
					DrawConfig draw;
					draw.model = +model;
					draw.shader = +shader;
					enc->draw(draw);
					*/
					enc->submit();
					device->submitCommandBuffers();
				}
				speaker->process(frame.frameDuration);
				window->processEvents();
			}
		}

		// unload assets
		assets->unload(AssetsName);
		assets->unloadCustomThread(0);

		// destroy window before the device
		window.clear();
		device.clear();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
