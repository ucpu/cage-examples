#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/macros.h>
#include <cage-core/string.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/scene.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

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
		TransformComponent &t = e->value<TransformComponent>();
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 0.5;
		c.ambientDirectionalColor = Vec3(1);
		c.ambientDirectionalIntensity = 0.5;
		c.effects = CameraEffectsFlags::Default;
	}
	{ // box 1
		Entity *e = ents->create(2);
		TransformComponent &t = e->value<TransformComponent>();
		RenderComponent &r = e->value<RenderComponent>();
		r.object = HashString("cage/model/fake.obj");
	}
}

bool guiUpdate();

bool update()
{
	uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();
	{ // box 1
		Entity *e = ents->get(2);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(sin(Rads(time * 1e-6)) * 10, cos(Rads(time * 1e-6)) * 10, -20);
	}
	if (updateDelay)
	{
		threadSleep(updateDelay);
	}
	guiUpdate();
	return false;
}

bool prepare()
{
	if (!prepareDelay)
		return false;
	threadSleep(prepareDelay);
	return false;
}

bool render()
{
	if (!renderDelay)
		return false;
	threadSleep(renderDelay);
	return false;
}

bool soundUpdate()
{
	if (!soundDelay)
		return false;
	threadSleep(soundDelay);
	return false;
}

bool guiInit()
{
	GuiManager *g = cage::engineGuiManager();

	Entity *panel = g->entities()->createUnique();
	{
		GuiScrollbarsComponent &sc = panel->value<GuiScrollbarsComponent>();
	}

	Entity *layout = g->entities()->createUnique();
	{
		GuiPanelComponent &c = layout->value<GuiPanelComponent>();
		GuiLayoutTableComponent &l = layout->value<GuiLayoutTableComponent>();
		GuiParentComponent &child = layout->value<GuiParentComponent>();
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
			GuiParentComponent &child = lab->value<GuiParentComponent>();
			child.parent = layout->name();
			child.order = i * 2 + 0;
			GuiLabelComponent &c = lab->value<GuiLabelComponent>();
			GuiTextComponent &t = lab->value<GuiTextComponent>();
			t.value = names[i];
		}
		Entity *con = g->entities()->create(20 + i);
		{
			GuiParentComponent &child = con->value<GuiParentComponent>();
			child.parent = layout->name();
			child.order = i * 2 + 1;
			GuiInputComponent &c = con->value<GuiInputComponent>();
			c.type = InputTypeEnum::Integer;
			c.min.i = i >= 2 ? 0 : 1;
			c.max.i = 1000;
			c.step.i = 1;
			c.value = Stringizer() + values[i];
		}
	}

	return false;
}

namespace
{
	void setIntValue(uint32 index, uint64 &value)
	{
		Entity *control = cage::engineGuiEntities()->get(20 + index);
		GuiInputComponent &t = control->value<GuiInputComponent>();
		if (t.valid)
		{
			CAGE_ASSERT(isDigitsOnly(t.value) && !t.value.empty());
			value = toUint32(t.value) * 1000;
		}
	}
}

bool guiUpdate()
{
	{
		Entity *control = cage::engineGuiEntities()->get(20 + 0);
		GuiInputComponent &t = control->value<GuiInputComponent>();
		if (t.valid)
		{
			CAGE_ASSERT(isDigitsOnly(t.value) && !t.value.empty());
			controlThread().updatePeriod(toUint32(t.value) * 1000);
		}
	}
	{
		Entity *control = cage::engineGuiEntities()->get(20 + 1);
		GuiInputComponent &t = control->value<GuiInputComponent>();
		if (t.valid)
		{
			CAGE_ASSERT(isDigitsOnly(t.value) && !t.value.empty());
			soundThread().updatePeriod(toUint32(t.value) * 1000);
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
		engineWindow()->windowedSize(Vec2i(800, 600));
		engineWindow()->title("timing frames");
		controlInit();
		Holder<StatisticsGui> statistics = newStatisticsGui();

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
