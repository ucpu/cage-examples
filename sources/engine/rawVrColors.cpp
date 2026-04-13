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
#include <cage-engine/virtualReality.h>
#include <cage-engine/window.h>

using namespace cage;

constexpr uint32 AssetsName = HashString("cage-tests/vr/vr.pack");

bool closing = false;

void headsetConnected(input::HeadsetConnected in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset connected");
}
void headsetDiconnected(input::HeadsetDisconnected in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset disconnected");
}
void headsetPose(input::HeadsetPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset pose: " + in.pose);
}
void controllerConnected(input::ControllerConnected in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller connected");
}
void controllerDiconnected(input::ControllerDisconnected in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller disconnected");
}
void controllerPose(input::ControllerPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller grip pose: " + in.pose);
}
void press(input::ControllerPress in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "press: " + in.key);
}
void release(input::ControllerRelease in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "release: " + in.key);
}
void axis(input::ControllerAxis in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "axis: " + in.axis + ", value: " + in.value);
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();

		Holder<Window> window = newWindow({});
		const auto closeListener = window->events.listen(inputFilter([](input::WindowClose) { closing = true; }));
		window->title("cage test virtual reality");

		// graphics device
		Holder<GraphicsDevice> device = newGraphicsDevice(GraphicsDeviceCreateConfig{ .compatibility = +window, .vsync = false });

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

		// vr
		Holder<VirtualReality> virtualreality = newVirtualReality();
		const auto headsetConnectedListener = virtualreality->events.listen(inputFilter(&headsetConnected));
		const auto headsetDisconnectedListener = virtualreality->events.listen(inputFilter(&headsetDiconnected));
		const auto headsetPoseListener = virtualreality->events.listen(inputFilter(&headsetPose));
		const auto controllerConnectedListener = virtualreality->events.listen(inputFilter(&controllerConnected));
		const auto controllerDisconnectedListener = virtualreality->events.listen(inputFilter(&controllerDiconnected));
		const auto controllerPoseListener = virtualreality->events.listen(inputFilter(&controllerPose));
		const auto pressListener = virtualreality->events.listen(inputFilter(press));
		const auto releaseListener = virtualreality->events.listen(inputFilter(release));
		const auto axisListener = virtualreality->events.listen(inputFilter(axis));

		{
			// fetch assets
			Holder<Model> model = assets->get<AssetSchemeIndexModel, Model>(HashString("cage/models/square.obj"));
			Holder<Texture> texture = assets->get<AssetSchemeIndexTexture, Texture>(HashString("cage-tests/logo/logo.png"));
			Holder<Shader> shader = assets->get<AssetSchemeIndexShader, MultiShader>(HashString("cage/shaders/engine/blitPixels.glsl"))->get(0);
			Holder<Sound> sound = assets->get<AssetSchemeIndexSound, Sound>(HashString("cage-tests/logo/logo.ogg"));

			// show the window
			window->windowedSize(Vec2i(600, 600));
			window->setWindowed();

			while (!closing)
			{
				const auto frame = device->nextFrame(+window);
				if (frame.targetTexture)
				{
					Holder<GraphicsEncoder> enc = newGraphicsEncoder(+device, "enc");
					RenderPassConfig pass;
					pass.colorTargets.push_back({ +frame.targetTexture });
					pass.colorTargets[0].clearValue = Vec4(randomChance(), 0, 0, 1);
					enc->nextPass(pass);
					enc->submit();
				}

				Holder<VirtualRealityGraphicsFrame> vrf = virtualreality->graphicsFrame();
				vrf->updateProjections();
				vrf->renderBegin();
				vrf->acquireTextures();
				uint32 index = 0;
				for (const auto &view : vrf->cameras)
				{
					if (!view.colorTexture)
						continue;
					Holder<GraphicsEncoder> enc = newGraphicsEncoder(+device, "enc");
					RenderPassConfig pass;
					pass.colorTargets.push_back({ view.colorTexture });
					pass.colorTargets[0].clearValue = Vec4(0, randomChance() * (index == 0), randomChance() * (index == 1), 1);
					enc->nextPass(pass);
					enc->submit();
					index++;
				}
				vrf->renderCommit();

				device->submitCommandBuffers();
				window->processEvents();
				virtualreality->processEvents();
			}
		}

		// unload assets
		assets->unload(AssetsName);
		assets->unloadCustomThread(0);

		// destroy window before the device
		virtualreality.clear();
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
