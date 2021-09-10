#include <cage-core/config.h>
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/window.h>
#include <cage-simple/fullscreenSwitcher.h>
#include <cage-simple/engine.h>

using namespace cage;

void windowClose(InputWindow);
void keyPress(InputKey in);
bool guiFunction(InputGuiWidget);
void update();

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

	EventListener<void()> updateListener;
	updateListener.attach(controlThread().update);
	updateListener.bind<&update>();
	InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
	closeListener.attach(engineWindow()->events);
	closeListener.bind<&windowClose>();
	InputListener<InputClassEnum::KeyPress, InputKey> keyPressListener;
	keyPressListener.attach(engineWindow()->events);
	keyPressListener.bind<&keyPress>();
	InputListener<InputClassEnum::GuiWidget, InputGuiWidget, bool> guiListener;
	guiListener.attach(engineGuiManager()->widgetEvent);
	guiListener.bind<&guiFunction>();

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
