#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/debug.h>

#include <cage-engine/window.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;
volatile bool fullStop = false;

constexpr float probInit = 0.05f;
constexpr float probLoop = 0.001f;
constexpr float probFinish = 0.1f;

void windowClose()
{
	fullStop = true;
	engineStop();
}

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
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(sin(rads(time * 1e-6)) * 10, cos(rads(time * 1e-6)) * 10, -20);
	}
	maybeThrow(probLoop, 3);
}

void prepareInit()
{
	maybeThrow(probInit, 5);
}

void prepareFinish()
{
	maybeThrow(probFinish, 6);
}

void preparePrepare()
{
	maybeThrow(probLoop, 7);
}

void dispatchInit()
{
	maybeThrow(probInit, 8);
}

void dispatchFinish()
{
	maybeThrow(probFinish, 9);
}

void dispatchRender()
{
	maybeThrow(probLoop, 10);
}

void dispatchSwap()
{
	maybeThrow(probLoop, 11);
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
		detail::setGlobalBreakpointOverride(false);

		// run
		while (!fullStop)
		{
			engineInitialize(EngineCreateConfig());

			// events
#define GCHL_GENERATE(FUNC, EVENT) EventListener<void()> FUNC##Listener; FUNC##Listener.bind<&FUNC>(); FUNC##Listener.attach(EVENT);
			GCHL_GENERATE(controlUpdate, controlThread().update);
			GCHL_GENERATE(controlInit, controlThread().initialize);
			GCHL_GENERATE(controlFinish, controlThread().finalize);
			GCHL_GENERATE(prepareInit, graphicsPrepareThread().initialize);
			GCHL_GENERATE(prepareFinish, graphicsPrepareThread().finalize);
			GCHL_GENERATE(preparePrepare, graphicsPrepareThread().prepare);
			GCHL_GENERATE(dispatchInit, graphicsDispatchThread().initialize);
			GCHL_GENERATE(dispatchFinish, graphicsDispatchThread().finalize);
			GCHL_GENERATE(dispatchRender, graphicsDispatchThread().dispatch);
			GCHL_GENERATE(dispatchSwap, graphicsDispatchThread().swap);
			GCHL_GENERATE(soundInit, soundThread().initialize);
			GCHL_GENERATE(soundFinish, soundThread().finalize);
			GCHL_GENERATE(soundSound, soundThread().sound);
#undef GCHL_GENERATE
			EventListener<void()> windowCloseListener;
			windowCloseListener.bind<&windowClose>();
			windowCloseListener.attach(engineWindow()->events.windowClose);

			engineWindow()->setWindowed();
			engineWindow()->windowedSize(ivec2(800, 600));
			engineWindow()->title("exceptions");

			{ // camera
				Entity *e = engineEntities()->create(1);
				CAGE_COMPONENT_ENGINE(Transform, t, e);
				CAGE_COMPONENT_ENGINE(Camera, c, e);
				c.ambientColor = vec3(1);
			}
			{ // box 1
				Entity *e = engineEntities()->create(2);
				CAGE_COMPONENT_ENGINE(Transform, t, e);
				CAGE_COMPONENT_ENGINE(Render, r, e);
				r.object = HashString("cage/model/fake.obj");
			}

			Holder<EngineProfiling> profiling = newEngineProfiling();

			engineStart();
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
