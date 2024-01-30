#include <cage-core/config.h>
#include <cage-core/logger.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fullscreenSwitcher.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

void keyPress(input::KeyPress in);
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

	const auto updateListener = controlThread().update.listen(&update);
	const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));
	const auto keyPressListener = engineWindow()->events.listen(inputFilter(keyPress));

	updateInitialize();
	{
		Holder<FullscreenSwitcher> fullscreen = newFullscreenSwitcher({});
		Holder<StatisticsGui> statistics = newStatisticsGui();
		engineRun();
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
