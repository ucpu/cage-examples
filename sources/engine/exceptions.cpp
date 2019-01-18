#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
volatile bool fullStop = false;

static const float probInit = 0.05f;
static const float probLoop = 0.001f;
static const float probFinish = 0.1f;

void windowClose()
{
	fullStop = true;
	engineStop();
}

void maybeThrow(float chance, uint32 index)
{
	if (randomChance() < chance)
		CAGE_THROW_WARNING(codeException, "intentionally thrown exception", index);
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
	uint64 time = currentControlTime();
	entityManagerClass *ents = entities();
	{
		entityClass *e = ents->get(2);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.position = vec3(sin(rads(time * 1e-6)) * 10, cos(rads(time * 1e-6)) * 10, -20);
	}
	maybeThrow(probLoop, 3);
}

void controlAssets()
{
	maybeThrow(probLoop, 4);
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
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		// override breakpoints
		detail::setGlobalBreakpointOverride(false);

		// config
		configSetBool("cage-client.engine.renderMissingMeshes", true);

		// run
		while (!fullStop)
		{
			engineInitialize(engineCreateConfig());

			// events
#define GCHL_GENERATE(FUNC, EVENT) eventListener<void()> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
			GCHL_GENERATE(controlUpdate, controlThread().update);
			GCHL_GENERATE(controlInit, controlThread().initialize);
			GCHL_GENERATE(controlFinish, controlThread().finalize);
			GCHL_GENERATE(controlAssets, controlThread().assets);
			GCHL_GENERATE(prepareInit, graphicsPrepareThread().initialize);
			GCHL_GENERATE(prepareFinish, graphicsPrepareThread().finalize);
			GCHL_GENERATE(preparePrepare, graphicsPrepareThread().prepare);
			GCHL_GENERATE(dispatchInit, graphicsDispatchThread().initialize);
			GCHL_GENERATE(dispatchFinish, graphicsDispatchThread().finalize);
			GCHL_GENERATE(dispatchRender, graphicsDispatchThread().render);
			GCHL_GENERATE(dispatchSwap, graphicsDispatchThread().swap);
			GCHL_GENERATE(soundInit, soundThread().initialize);
			GCHL_GENERATE(soundFinish, soundThread().finalize);
			GCHL_GENERATE(soundSound, soundThread().sound);
#undef GCHL_GENERATE
			eventListener<void()> windowCloseListener;
			windowCloseListener.bind<&windowClose>();
			windowCloseListener.attach(window()->events.windowClose);

			window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
			window()->windowedSize(pointStruct(800, 600));
			window()->title("exceptions");

			{ // camera
				entityClass *e = entities()->create(1);
				ENGINE_GET_COMPONENT(transform, t, e);
				ENGINE_GET_COMPONENT(camera, c, e);
				c.ambientLight = vec3(1, 1, 1);
			}
			{ // box 1
				entityClass *e = entities()->create(2);
				ENGINE_GET_COMPONENT(transform, t, e);
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = 1; // something non-existing
			}

			holder<engineProfilingClass> profiling = newEngineProfiling();

			engineStart();
			engineFinalize();
		}

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
