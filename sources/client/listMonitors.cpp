#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/graphics.h>
#include <cage-engine/screenList.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

void testScreen(const string &screenId, const ivec2 &resolution, uint32 frequency)
{
	CAGE_LOG(severityEnum::Info, "test", string() + "monitor: '" + screenId + "', resolution: " + resolution.x + " * " + resolution.y + ", frequency: " + frequency);
	{
		holder<windowHandle> w = newWindow();
		w->setFullscreen(resolution, frequency, screenId);
		w->title("cage test monitors");
		w->processEvents();
		detail::initializeOpengl();
		w->swapBuffers();
		w->processEvents();
		glViewport(0, 0, resolution.x, resolution.y);
		glClearColor(randomChance().value, randomChance().value, randomChance().value, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		w->swapBuffers();
		w->processEvents();
		threadSleep(2 * 1000 * 1000);
	}
	threadSleep(500 * 1000);
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		holder<screenList> list = newScreenList();
		for (uint32 i = 0; i < list->devicesCount(); i++)
		{
			CAGE_LOG(severityEnum::Info, "listing", string() + "monitor" + (list->defaultDevice() == i ? ", primary" : ""));
			const screenDevice *d = list->device(i);
			CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "monitor id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "monitor name: '" + d->name() + "'");

			screenMode testmode;

			for (uint32 j = 0; j < d->modesCount(); j++)
			{
				const screenMode &m = d->mode(j);
				CAGE_LOG(severityEnum::Info, "listing", string() + "video mode " + j + (d->currentMode() == j ? ", current" : ""));
				ivec2 r = m.resolution;
				CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "resolution: " + r.x + "*" + r.y);
				CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "frequency: " + m.frequency);
				if (d->currentMode() == j)
					testmode = m;
			}

			testScreen(d->id(), testmode.resolution, testmode.frequency);
		}

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
