#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/utility/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/engine.h>
#include <cage-client/utility/highPerformanceGpuHint.h>

using namespace cage;
volatile bool fullStop = false;

static const float probInit = 0.05f;
static const float probLoop = 0.001f;
static const float probFinish = 0.1f;

bool windowClose()
{
	fullStop = true;
	engineStop();
	return false;
}

void maybeThrow(float chance, uint32 index)
{
	if (random() < chance)
		CAGE_THROW_ERROR(codeException, "intentionally thrown exception", index);
}

bool controlInit()
{
	maybeThrow(probInit, 1);
	return false;
}

bool controlFinish()
{
	maybeThrow(probFinish, 2);
	return false;
}

bool controlUpdate()
{
	uint64 time = currentControlTime();
	entityManagerClass *ents = entities();
	{
		entityClass *e = ents->getEntity(2);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.position = vec3(sin(rads(time * 1e-6)) * 10, cos(rads(time * 1e-6)) * 10, -20);
	}
	maybeThrow(probLoop, 3);
	return false;
}

bool controlAssets()
{
	maybeThrow(probLoop, 4);
	return false;
}

bool prepareInit()
{
	maybeThrow(probInit, 5);
	return false;
}

bool prepareFinish()
{
	maybeThrow(probFinish, 6);
	return false;
}

bool preparePrepare()
{
	maybeThrow(probLoop, 7);
	return false;
}

bool dispatchInit()
{
	maybeThrow(probInit, 8);
	return false;
}

bool dispatchFinish()
{
	maybeThrow(probFinish, 9);
	return false;
}

bool dispatchRender()
{
	maybeThrow(probLoop, 10);
	return false;
}

bool dispatchSwap()
{
	maybeThrow(probLoop, 11);
	return false;
}

bool soundInit()
{
	maybeThrow(probInit, 12);
	return false;
}

bool soundFinish()
{
	maybeThrow(probFinish, 13);
	return false;
}

bool soundSound()
{
	maybeThrow(probLoop, 14);
	return false;
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
		controlThread().timePerTick = 1000000 / 60;

		// run
		while (!fullStop)
		{
			engineInitialize(engineCreateConfig());

			// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
			GCHL_GENERATE((), controlUpdate, controlThread().update);
			GCHL_GENERATE((), controlInit, controlThread().initialize);
			GCHL_GENERATE((), controlFinish, controlThread().finalize);
			GCHL_GENERATE((), controlAssets, controlThread().assets);
			GCHL_GENERATE((), prepareInit, graphicsPrepareThread().initialize);
			GCHL_GENERATE((), prepareFinish, graphicsPrepareThread().finalize);
			GCHL_GENERATE((), preparePrepare, graphicsPrepareThread().prepare);
			GCHL_GENERATE((), dispatchInit, graphicsDispatchThread().initialize);
			GCHL_GENERATE((), dispatchFinish, graphicsDispatchThread().finalize);
			GCHL_GENERATE((), dispatchRender, graphicsDispatchThread().render);
			GCHL_GENERATE((), dispatchSwap, graphicsDispatchThread().swap);
			GCHL_GENERATE((), soundInit, soundThread().initialize);
			GCHL_GENERATE((), soundFinish, soundThread().finalize);
			GCHL_GENERATE((), soundSound, soundThread().sound);
#undef GCHL_GENERATE
			eventListener<bool()> windowCloseListener;
			windowCloseListener.bind<&windowClose>();
			windowCloseListener.attach(window()->events.windowClose);

			window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
			window()->windowedSize(pointStruct(800, 600));
			window()->title("cage test engine exceptions");

			{ // camera
				entityClass *e = entities()->newEntity(1);
				ENGINE_GET_COMPONENT(transform, t, e);
				(void)t;
				ENGINE_GET_COMPONENT(camera, c, e);
				c.ambientLight = vec3(1, 1, 1);
			}
			{ // box 1
				entityClass *e = entities()->newEntity(2);
				ENGINE_GET_COMPONENT(transform, t, e);
				(void)t;
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = 1; // something non-existing
			}

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
