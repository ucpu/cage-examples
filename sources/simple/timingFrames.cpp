#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/hashString.h>
#include <cage-core/string.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

uint64 updateDelay = 0;
uint64 prepareDelay = 0;
uint64 renderDelay = 0;
uint64 soundDelay = 0;

struct ShotComponent
{
	uint32 ttl = 20;
};

void shoot(const Transform &where)
{
	auto ents = engineEntities();
	Entity *e = ents->createAnonymous();
	e->value<TransformComponent>() = where;
	e->value<TransformComponent>().scale = 0.25;
	e->value<ShotComponent>();
	e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
}

void windowClose(InputWindow)
{
	engineStop();
}

void controlInit()
{
	EntityManager *ents = engineEntities();
	ents->defineComponent(ShotComponent());
	{ // camera
		Entity *e = ents->create(1);
		e->value<TransformComponent>();
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 1;
		e->value<ScreenSpaceEffectsComponent>();
	}
	{ // box 1
		Entity *e = ents->create(2);
		e->value<TransformComponent>();
		e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
	}
}

void guiUpdate();

void update()
{
	const uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();
	{ // box 1
		Entity *e = ents->get(2);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(sin(Rads(time * 1e-6)) * 10, cos(Rads(time * 1e-6)) * 10, -20);
	}
	entitiesVisitor([&](Entity *e, TransformComponent &t, ShotComponent &s) {
		if (s.ttl-- == 0)
			e->destroy();
		else
			t.position += t.orientation * Vec3(0, 0, -2);
	}, ents, true);
	shoot(Transform(Vec3(-20, 0, -25), Quat(Degs(), Degs(-90), Degs())));
	if (updateDelay)
		threadSleep(updateDelay);
	guiUpdate();
}

void prepare()
{
	if (!prepareDelay)
		return;
	threadSleep(prepareDelay);
}

void render()
{
	if (!renderDelay)
		return;
	threadSleep(renderDelay);
}

void soundUpdate()
{
	if (!soundDelay)
		return;
	threadSleep(soundDelay);
}

void guiInit()
{
	GuiManager *g = cage::engineGuiManager();

	Entity *panel = g->entities()->createUnique();
	{
		panel->value<GuiLayoutScrollbarsComponent>();
		panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0);
	}

	Entity *layout = g->entities()->createUnique();
	{
		layout->value<GuiPanelComponent>();
		layout->value<GuiLayoutTableComponent>();
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
			lab->value<GuiLabelComponent>();
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

void guiUpdate()
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
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		EventListener<void()> prepareListener;
		prepareListener.attach(graphicsPrepareThread().prepare);
		prepareListener.bind<&prepare>();
		EventListener<void()> renderListener;
		renderListener.attach(graphicsDispatchThread().dispatch);
		renderListener.bind<&render>();
		EventListener<void()> soundListener;
		soundListener.attach(soundThread().sound);
		soundListener.bind<&soundUpdate>();
		EventListener<void()> guiInitListener;
		guiInitListener.attach(controlThread().initialize);
		guiInitListener.bind<&guiInit>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		engineWindow()->setMaximized();
		engineWindow()->title("timing frames");
		controlInit();
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineRun();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
