#include "game.h"
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/fullscreenSwitcher.h>
#include <cage-core/config.h>

bool closeButton();
bool keyPress(uint32 a, uint32 b, ModifiersFlags m);
bool guiFunction(uint32 en);
bool update();
bool assetsUpdate();

void cameraInitialize();
void updateInitialize();
void updateFinalize();

int main(int argc, const char *args[])
{
	// log to console
	Holder<Logger> log1 = newLogger();
	log1->format.bind<logFormatConsole>();
	log1->output.bind<logOutputStdOut>();

	configSetBool("cage/config/autoSave", true);
	engineInitialize(EngineCreateConfig());
	cameraInitialize();

	EventListener<bool()> updateListener;
	EventListener<bool()> assetsUpdateListener;
	WindowEventListeners listeners;
	EventListener<bool(uint32)> guiListener;
	updateListener.bind<&update>();
	assetsUpdateListener.bind<&assetsUpdate>();
	controlThread().update.attach(updateListener);
	controlThread().assets.attach(assetsUpdateListener);
	listeners.windowClose.bind<&closeButton>();
	listeners.keyPress.bind<&keyPress>();
	listeners.attachAll(engineWindow());
	guiListener.bind<&guiFunction>();
	engineGui()->widgetEvent.attach(guiListener);

	updateInitialize();
	{
		Holder<FullscreenSwitcher> fullscreen = newFullscreenSwitcher(false);
		engineStart();
	}
	updateFinalize();
	engineFinalize();

	{
		Holder<ConfigList> l = newConfigList();
		while (l->valid())
		{
			CAGE_LOG(SeverityEnum::Info, "config", stringizer() + l->name() + " = " + l->getString() + " (" + l->typeName() + ")");
			l->next();
		}
	}

	return 0;
}
