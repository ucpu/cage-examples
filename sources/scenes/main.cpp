#include "game.h"
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/fullscreenSwitcher.h>
#include <cage-core/config.h>

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
	holder<logger> log1 = newLogger();
	log1->format.bind<logFormatConsole>();
	log1->output.bind<logOutputStdOut>();

	configSetBool("cage.config.autoSave", true);
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
	listeners.windowClose.bind<&closeButton>();
	listeners.keyPress.bind<&keyPress>();
	listeners.attachAll(window());
	guiListener.bind<&guiFunction>();
	gui()->widgetEvent.attach(guiListener);

	updateInitialize();
	{
		holder<fullscreenSwitcher> fullscreen = newFullscreenSwitcher(false);
		engineStart();
	}
	updateFinalize();
	engineFinalize();

	{
		holder<configList> l = newConfigList();
		while (l->valid())
		{
			CAGE_LOG(severityEnum::Info, "config", string() + l->name() + " = " + l->getString() + " (" + l->typeName() + ")");
			l->next();
		}
	}

	return 0;
}
