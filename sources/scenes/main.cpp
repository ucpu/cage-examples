#include "game.h"
#include <cage-client/highPerformanceGpuHint.h>

bool closeButton();
bool keyPress(uint32 a, uint32 b, modifiersFlags m);
bool guiFunction(uint32 en);
bool update();
bool assetsUpdate();

void cameraInitialize();
void updateInitialize();
void updateFinalize();

int main(int argc, const char *args[])
{
	// log to console
	holder<loggerClass> log1 = newLogger();
	log1->filter.bind<logFilterPolicyPass>();
	log1->format.bind<logFormatPolicyConsole>();
	log1->output.bind<logOutputPolicyStdOut>();

	engineInitialize(engineCreateConfig());
	cameraInitialize();

	eventListener<bool()> updateListener;
	eventListener<bool()> assetsUpdateListener;
	windowEventListeners listeners;
	eventListener<bool(uint32)> guiListener;

	updateListener.bind<&update>();
	assetsUpdateListener.bind<&assetsUpdate>();
	controlThread().update.attach(updateListener);
	controlThread().assets.attach(assetsUpdateListener);

	{
		windowClass *win = window();
		win->setWindowed();
		win->windowedSize(pointStruct(800, 600));
		listeners.windowClose.bind<&closeButton>();
		listeners.keyPress.bind<&keyPress>();
		listeners.attachAll(window());
	}

	guiListener.bind<&guiFunction>();
	gui()->widgetEvent.attach(guiListener);

	updateInitialize();
	engineStart();
	updateFinalize();
	engineFinalize();

	{
		holder<configListClass> l = newConfigList();
		while (l->valid())
		{
			CAGE_LOG(severityEnum::Info, "config", string() + l->name() + " = " + l->getString() + " (" + l->typeName() + ")");
			l->next();
		}
	}

	return 0;
}
