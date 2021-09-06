#include <cage-core/config.h>
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/window.h>
#include <cage-simple/fullscreenSwitcher.h>
#include <cage-simple/engine.h>

using namespace cage;

bool closeButton();
bool keyPress(uint32 a, ModifiersFlags m);
bool guiFunction(uint32 en);
bool update();

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
	WindowEventListeners listeners;
	EventListener<bool(uint32)> guiListener;
	updateListener.bind<&update>();
	controlThread().update.attach(updateListener);
	listeners.windowClose.bind<&closeButton>();
	listeners.keyPress.bind<&keyPress>();
	listeners.attachAll(engineWindow());
	guiListener.bind<&guiFunction>();
	engineGuiManager()->widgetEvent.attach(guiListener);

	updateInitialize();
	{
		Holder<FullscreenSwitcher> fullscreen = newFullscreenSwitcher({});
		engineStart();
	}
	updateFinalize();
	engineFinalize();

	{
		Holder<ConfigList> l = newConfigList();
		while (l->valid())
		{
			CAGE_LOG(SeverityEnum::Info, "config", Stringizer() + l->name() + " = " + l->getString() + " (" + l->typeName() + ")");
			l->next();
		}
	}

	return 0;
}
