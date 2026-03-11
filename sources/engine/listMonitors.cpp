#include <cage-core/concurrent.h>
#include <cage-core/logger.h>
#include <cage-engine/graphicsDevice.h>
#include <cage-engine/graphicsEncoder.h>
#include <cage-engine/screenList.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>

using namespace cage;

void testScreen(const String &screenId, const Vec2i &resolution, uint32 frequency)
{
	CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "testing monitor: '" + screenId + "', resolution: " + resolution[0] + " * " + resolution[1] + ", frequency: " + frequency);
	{
		Vec3 color = randomChance3() * 0.5 + 0.5;
		Holder<Window> window = newWindow({});
		Holder<GraphicsDevice> device = newGraphicsDevice({ +window });
		window->setFullscreen(resolution, frequency, screenId);
		window->title("cage test list monitors");
		window->processEvents();
		for (int i = 0; i < 30; i++)
		{
			const auto frame = device->nextFrame(+window);
			if (frame.targetTexture)
			{
				Vec2i res = frame.targetTexture->resolution();
				Holder<GraphicsEncoder> enc = newGraphicsEncoder(+device, "enc");
				RenderPassConfig pass;
				pass.colorTargets.push_back({ +frame.targetTexture });
				pass.colorTargets[0].clearValue = Vec4(randomChance3(), 1);
				enc->nextPass(pass);
				enc->submit();
				device->submitCommandBuffers();
			}
			window->processEvents();
			threadSleep(100'000);
		}
		window.clear();
		device.clear();
	}
	threadSleep(500 * 1000);
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();

		Holder<ScreenList> list = newScreenList();
		uint32 defaultIndex = list->defaultDevice();
		for (const ScreenDevice *d : list->devices())
		{
			CAGE_LOG(SeverityEnum::Info, "listing", Stringizer() + "monitor" + (defaultIndex-- == 0 ? ", primary" : "") + ":");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "monitor id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "monitor name: '" + d->name() + "'");

			ScreenMode testmode;
			uint32 currentIndex = d->currentMode();
			for (const ScreenMode &m : d->modes())
			{
				if (currentIndex == 0)
					testmode = m;
				CAGE_LOG(SeverityEnum::Info, "listing", Stringizer() + "video mode " + (currentIndex-- == 0 ? ", current" : "") + ":");
				Vec2i r = m.resolution;
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "resolution: " + r[0] + "*" + r[1]);
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "frequency: " + m.frequency);
			}

			testScreen(d->id(), testmode.resolution, testmode.frequency);
		}

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
