#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>

#include <cage-engine/window.h>
#include <cage-engine/screenList.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

void testScreen(const string &screenId, const ivec2 &resolution, uint32 frequency)
{
	CAGE_LOG(SeverityEnum::Info, "test", stringizer() + "testing monitor: '" + screenId + "', resolution: " + resolution[0] + " * " + resolution[1] + ", frequency: " + frequency);
	{
		vec3 color = randomChance3() * 0.5 + 0.5;
		Holder<Window> w = newWindow();
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
			CAGE_LOG(SeverityEnum::Info, "listing", stringizer() + "monitor" + (defaultIndex-- == 0 ? ", primary" : "") + ":");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "monitor id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "monitor name: '" + d->name() + "'");

			ScreenMode testmode;

			uint32 currentIndex = d->currentMode();
			for (const ScreenMode &m : d->modes())
			{
				if (currentIndex == 0)
					testmode = m;
				CAGE_LOG(SeverityEnum::Info, "listing", stringizer() + "video mode " + (currentIndex-- == 0 ? ", current" : "") + ":");
				ivec2 r = m.resolution;
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "resolution: " + r[0] + "*" + r[1]);
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "frequency: " + m.frequency);
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
