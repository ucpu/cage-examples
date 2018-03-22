#include "game.h"
#include <cage-client/utility/highPerformanceGpuHint.h>

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
	engineInitialize(engineCreateConfig());
	cameraInitialize();

	eventListener<bool()> updateListener;
	eventListener<bool()> assetsUpdateListener;
	windowEventListeners listeners;
	eventListener<bool(uint32)> guiListener;

	updateListener.bind<&update>();
	assetsUpdateListener.bind<&assetsUpdate>();
	listeners.windowClose.bind<&closeButton>();
	listeners.keyPress.bind<&keyPress>();
	guiListener.bind<&guiFunction>();

	{
		windowClass *win = window();
		win->modeSetWindowed(windowFlags::Border | windowFlags::Resizeable);
		win->windowedSize(pointStruct(800, 600));
		controlThread::update.attach(updateListener);
		controlThread::assets.attach(assetsUpdateListener);
		listeners.attachAll(window());
	}

	gui()->widgetEvent.attach(guiListener);

	updateInitialize();
	engineStart();
	updateFinalize();
	engineFinalize();

	{
		holder<configListClass> l = newConfigList();
		while (l->valid())
		{
			CAGE_LOG(severityEnum::Info, "config", string() + l->typeName() + " " + l->name() + ": " + l->getString());
			l->next();
		}
	}

	return 0;
}
