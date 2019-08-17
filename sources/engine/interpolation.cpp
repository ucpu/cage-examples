#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
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
	entityManager *ents = entities();
	{ // camera
		entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(camera, c, e);
		c.ambientLight = vec3(1, 1, 1);
		c.effects = cameraEffectsFlags::CombinedPass;
	}
	{ // box 1
		entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(render, r, e);
		r.object = hashString("cage/mesh/fake.obj");
	}
}

bool guiUpdate();

bool update()
{
	uint64 time = currentControlTime();
	entityManager *ents = entities();
	{ // box 1
		entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(transform, t, e);
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
	guiManager *g = cage::gui();

	entity *panel = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(scrollbars, sc, panel);
	}

	entity *layout = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(panel, c, layout);
		CAGE_COMPONENT_GUI(layoutTable, l, layout);
		CAGE_COMPONENT_GUI(parent, child, layout);
		child.parent = panel->name();
	}

	// controls
	static const string names[] = { "control tick", "sound tick", "control delay", "prepare delay", "dispatch delay", "sound delay" };
	static const uint64 values[] = { 1000 / 30, 1000 / 60, 0, 0, 0, 0 };
	CAGE_ASSERT_COMPILE(sizeof(names) / sizeof(names[0]) == sizeof(values) / sizeof(values[0]), arrays_must_have_same_length);
	for (uint32 i = 0; i < sizeof(names) / sizeof(names[0]); i++)
	{
		entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, child, lab);
			child.parent = layout->name();
			child.order = i * 2 + 0;
			CAGE_COMPONENT_GUI(label, c, lab);
			CAGE_COMPONENT_GUI(text, t, lab);
			t.value = names[i];
		}
		entity *con = g->entities()->create(20 + i);
		{
			CAGE_COMPONENT_GUI(parent, child, con);
			child.parent = layout->name();
			child.order = i * 2 + 1;
			CAGE_COMPONENT_GUI(input, c, con);
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
		entity *control = cage::gui()->entities()->get(20 + index);
		CAGE_COMPONENT_GUI(input, t, control);
		if (t.valid)
		{
			CAGE_ASSERT(t.value.isDigitsOnly() && !t.value.empty());
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
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

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

		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("interpolation");
		controlInit();
		holder<engineProfiling> engineProfiling = newEngineProfiling();

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
