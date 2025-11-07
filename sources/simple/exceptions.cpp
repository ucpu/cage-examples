#include <cage-core/concurrent.h>
#include <cage-core/debug.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
volatile bool fullStop = false;

constexpr float probInit = 0.05f;
constexpr float probLoop = 0.001f;
constexpr float probFinish = 0.1f;

void maybeThrow(float chance, uint32 index)
{
	if (randomChance() < chance)
		CAGE_THROW_SILENT(SystemError, "intentionally thrown exception", index);
}

void controlInit()
{
	maybeThrow(probInit, 1);
}

void controlFinish()
{
	maybeThrow(probFinish, 2);
}

void controlUpdate()
{
	uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();
	{
		Entity *e = ents->get(2);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(sin(Rads(time * 1e-6)) * 10, cos(Rads(time * 1e-6)) * 10, -20);
	}
	maybeThrow(probLoop, 3);
}

void graphicsInit()
{
	maybeThrow(probInit, 8);
}

void graphicsFinish()
{
	maybeThrow(probFinish, 9);
}

void graphicsRender()
{
	maybeThrow(probLoop, 10);
}

void soundInit()
{
	maybeThrow(probInit, 12);
}

void soundFinish()
{
	maybeThrow(probFinish, 13);
}

void soundSound()
{
	maybeThrow(probLoop, 14);
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		// override breakpoints
		detail::globalBreakpointOverride(false);

		// run
		while (!fullStop)
		{
			engineInitialize(EngineCreateConfig());

			// events
#define GCHL_GENERATE(FUNC, EVENT) const EventListener<bool()> FUNC##Listener = EVENT.listen(&FUNC);
			GCHL_GENERATE(controlUpdate, controlThread().update);
			GCHL_GENERATE(controlInit, controlThread().initialize);
			GCHL_GENERATE(controlFinish, controlThread().finalize);
			GCHL_GENERATE(graphicsInit, graphicsThread().initialize);
			GCHL_GENERATE(graphicsFinish, graphicsThread().finalize);
			GCHL_GENERATE(graphicsRender, graphicsThread().graphics);
			GCHL_GENERATE(soundInit, soundThread().initialize);
			GCHL_GENERATE(soundFinish, soundThread().finalize);
			GCHL_GENERATE(soundSound, soundThread().sound);
#undef GCHL_GENERATE
			const auto closeListener = engineWindow()->events.listen(inputFilter(
				[](input::WindowClose)
				{
					fullStop = true;
					engineStop();
				}));

			engineWindow()->setWindowed();
			engineWindow()->windowedSize(Vec2i(800, 600));
			engineWindow()->title("exceptions");

			{ // camera
				Entity *e = engineEntities()->create(1);
				e->value<TransformComponent>();
				CameraComponent &c = e->value<CameraComponent>();
				c.ambientColor = Vec3(1);
			}
			{ // box 1
				Entity *e = engineEntities()->create(2);
				e->value<TransformComponent>();
				e->value<ModelComponent>().model = HashString("cage/models/fake.obj");
			}

			Holder<StatisticsGui> statistics = newStatisticsGui();

			engineRun();
			engineFinalize();
		}

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
