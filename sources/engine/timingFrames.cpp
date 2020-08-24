#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <optick.h>

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
	EntityManager *ents = engineEntities();
	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientColor = vec3(1);
		c.ambientIntensity = 0.5;
		c.ambientDirectionalColor = vec3(1);
		c.ambientDirectionalIntensity = 0.5;
		c.effects = CameraEffectsFlags::CombinedPass;
	}
	{ // box 1
		Entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage/mesh/fake.obj");
	}
}

bool guiUpdate();

bool update()
{
	uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();
	{ // box 1
		Entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(sin(rads(time * 1e-6)) * 10, cos(rads(time * 1e-6)) * 10, -20);
	}
	if (updateDelay)
	{
		OPTICK_EVENT("updateDelay");
		OPTICK_TAG("updateDelay", updateDelay);
		threadSleep(updateDelay);
	}
	guiUpdate();
	return false;
}

bool prepare()
{
	if (!prepareDelay)
		return false;
	OPTICK_EVENT("prepareDelay");
	OPTICK_TAG("prepareDelay", prepareDelay);
	threadSleep(prepareDelay);
	return false;
}

bool render()
{
	if (!renderDelay)
		return false;
	OPTICK_EVENT("renderDelay");
	OPTICK_TAG("renderDelay", renderDelay);
	threadSleep(renderDelay);
	return false;
}

bool soundUpdate()
{
	if (!soundDelay)
		return false;
	OPTICK_EVENT("soundDelay");
	OPTICK_TAG("soundDelay", soundDelay);
	threadSleep(soundDelay);
	return false;
}

bool guiInit()
{
	Gui *g = cage::engineGui();

	Entity *panel = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
	}

	Entity *layout = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(Panel, c, layout);
		CAGE_COMPONENT_GUI(LayoutTable, l, layout);
		CAGE_COMPONENT_GUI(Parent, child, layout);
		child.parent = panel->name();
	}

	// controls
	constexpr const char *names[] = { "control tick", "sound tick", "control delay", "prepare delay", "dispatch delay", "sound delay" };
	constexpr const uint64 values[] = { 1000 / 30, 1000 / 60, 0, 0, 0, 0 };
	static_assert(sizeof(names) / sizeof(names[0]) == sizeof(values) / sizeof(values[0]), "arrays must have same length");
	for (uint32 i = 0; i < sizeof(names) / sizeof(names[0]); i++)
	{
		Entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, child, lab);
			child.parent = layout->name();
			child.order = i * 2 + 0;
			CAGE_COMPONENT_GUI(Label, c, lab);
			CAGE_COMPONENT_GUI(Text, t, lab);
			t.value = names[i];
		}
		Entity *con = g->entities()->create(20 + i);
		{
			CAGE_COMPONENT_GUI(Parent, child, con);
			child.parent = layout->name();
			child.order = i * 2 + 1;
			CAGE_COMPONENT_GUI(Input, c, con);
			c.type = InputTypeEnum::Integer;
			c.min.i = i >= 2 ? 0 : 1;
			c.max.i = 1000;
			c.step.i = 1;
			c.value = string(values[i]);
		}
	}

	return false;
}

namespace
{
	void setIntValue(uint32 index, uint64 &value)
	{
		Entity *control = cage::engineGui()->entities()->get(20 + index);
		CAGE_COMPONENT_GUI(Input, t, control);
		if (t.valid)
		{
			CAGE_ASSERT(t.value.isDigitsOnly() && !t.value.empty());
			value = t.value.toUint32() * 1000;
		}
	}
}

bool guiUpdate()
{
	{
		Entity *control = cage::engineGui()->entities()->get(20 + 0);
		CAGE_COMPONENT_GUI(Input, t, control);
		if (t.valid)
		{
			CAGE_ASSERT(t.value.isDigitsOnly() && !t.value.empty());
			controlThread().updatePeriod(t.value.toUint32() * 1000);
		}
	}
	{
		Entity *control = cage::engineGui()->entities()->get(20 + 1);
		CAGE_COMPONENT_GUI(Input, t, control);
		if (t.valid)
		{
			CAGE_ASSERT(t.value.isDigitsOnly() && !t.value.empty());
			soundThread().updatePeriod(t.value.toUint32() * 1000);
		}
	}
	setIntValue(2, updateDelay);
	setIntValue(3, prepareDelay);
	setIntValue(4, renderDelay);
	setIntValue(5, soundDelay);
	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(EngineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), prepare, graphicsPrepareThread().prepare);
		GCHL_GENERATE((), render, graphicsDispatchThread().dispatch);
		GCHL_GENERATE((), soundUpdate, soundThread().sound);
		GCHL_GENERATE((), guiInit, controlThread().initialize);
#undef GCHL_GENERATE
		EventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(engineWindow()->events.windowClose);

		engineWindow()->setWindowed();
		engineWindow()->windowedSize(ivec2(800, 600));
		engineWindow()->title("timing frames");
		controlInit();
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
