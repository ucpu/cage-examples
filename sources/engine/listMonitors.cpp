#include <cage-core/concurrent.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>

#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/opengl.h>
#include <cage-engine/screenList.h>
#include <cage-engine/window.h>

using namespace cage;

void testScreen(const String &screenId, const Vec2i &resolution, uint32 frequency)
{
	CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "testing monitor: '" + screenId + "', resolution: " + resolution[0] + " * " + resolution[1] + ", frequency: " + frequency);
	{
		Vec3 color = randomChance3() * 0.5 + 0.5;
		Holder<Window> w = newWindow({});
		w->setFullscreen(resolution, frequency, screenId);
		w->title("cage test monitors");
		w->processEvents();
		detail::initializeOpengl();
		for (int i = 0; i < 100; i++)
		{
			glViewport(0, 0, resolution[0], resolution[1]);
			glClearColor(color[0].value, color[1].value, color[2].value, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			w->swapBuffers();
			w->processEvents();
			threadSleep(100000);
		}
	}
	threadSleep(500 * 1000);
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

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
