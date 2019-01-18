#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

uint64 updateDelay = 0;
uint64 prepareDelay = 0;
uint64 renderDelay = 0;
uint64 soundDelay = 0;

bool windowClose()
{
	engineStop();
	return false;
}

void controlInit()
{
	entityManagerClass *ents = entities();
	{ // camera
		entityClass *e = ents->create(1);
		ENGINE_GET_COMPONENT(transform, t, e);
		(void)t;
		ENGINE_GET_COMPONENT(camera, c, e);
		c.ambientLight = vec3(1, 1, 1);
		c.effects = cameraEffectsFlags::CombinedPass;
	}
	{ // box 1
		entityClass *e = ents->create(2);
		ENGINE_GET_COMPONENT(transform, t, e);
		(void)t;
		ENGINE_GET_COMPONENT(render, r, e);
		r.object = 1; // something non-existing
	}
}

bool guiUpdate();

bool update()
{
	uint64 time = currentControlTime();
	entityManagerClass *ents = entities();
	{ // box 1
		entityClass *e = ents->get(2);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.position = vec3(sin(rads(time * 1e-6)) * 10, cos(rads(time * 1e-6)) * 10, -20);
	}
	threadSleep(updateDelay);
	guiUpdate();
	return false;
}

bool prepare()
{
	threadSleep(prepareDelay);
	return false;
}

bool render()
{
	threadSleep(renderDelay);
	return false;
}

bool soundUpdate()
{
	threadSleep(soundDelay);
	return false;
}

bool guiInit()
{
	guiClass *g = cage::gui();

	entityClass *panel = g->entities()->createUnique();
	{
		GUI_GET_COMPONENT(scrollbars, sc, panel);
	}

	entityClass *layout = g->entities()->createUnique();
	{
		GUI_GET_COMPONENT(panel, c, layout);
		GUI_GET_COMPONENT(layoutTable, l, layout);
		GUI_GET_COMPONENT(parent, child, layout);
		child.parent = panel->name();
	}

	// controls
	static const string names[] = { "control tick", "sound tick", "control delay", "prepare delay", "dispatch delay", "sound delay" };
	static const uint64 values[] = { 1000 / 30, 1000 / 60, 0, 0, 0, 0 };
	CAGE_ASSERT_COMPILE(sizeof(names) / sizeof(names[0]) == sizeof(values) / sizeof(values[0]), arrays_must_have_same_length);
	for (uint32 i = 0; i < sizeof(names) / sizeof(names[0]); i++)
	{
		entityClass *lab = g->entities()->createUnique();
		{
			GUI_GET_COMPONENT(parent, child, lab);
			child.parent = layout->name();
			child.order = i * 2 + 0;
			GUI_GET_COMPONENT(label, c, lab);
			GUI_GET_COMPONENT(text, t, lab);
			t.value = names[i];
		}
		entityClass *con = g->entities()->create(20 + i);
		{
			GUI_GET_COMPONENT(parent, child, con);
			child.parent = layout->name();
			child.order = i * 2 + 1;
			GUI_GET_COMPONENT(input, c, con);
			c.type = inputTypeEnum::Integer;
			c.min.i = i >= 2 ? 0 : 1;
			c.max.i = 1000;
			c.step.i = 1;
			c.value = values[i];
		}
	}

	return false;
}

namespace
{
	void setIntValue(uint32 index, uint64 &value, bool allowZero)
	{
		entityClass *control = cage::gui()->entities()->get(20 + index);
		GUI_GET_COMPONENT(input, t, control);
		if (t.valid)
		{
			CAGE_ASSERT_RUNTIME(t.value.isDigitsOnly() && !t.value.empty());
			value = t.value.toUint32() * 1000;
		}
	}
}

bool guiUpdate()
{
	setIntValue(0, controlThread().timePerTick, false);
	setIntValue(1, soundThread().timePerTick, false);
	setIntValue(2, updateDelay, true);
	setIntValue(3, prepareDelay, true);
	setIntValue(4, renderDelay, true);
	setIntValue(5, soundDelay, true);
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

		configSetBool("cage-client.engine.renderMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), prepare, graphicsPrepareThread().prepare);
		GCHL_GENERATE((), render, graphicsDispatchThread().render);
		GCHL_GENERATE((), soundUpdate, soundThread().sound);
		GCHL_GENERATE((), guiInit, controlThread().initialize);
#undef GCHL_GENERATE
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(window()->events.windowClose);

		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("interpolation");
		controlInit();
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
