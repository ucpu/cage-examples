#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/threadPool.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <atomic>

using namespace cage;

Holder<FpsCamera> cameraCtrl;
Holder<ThreadPool> updateThreads;
Holder<NoiseFunction> noise;
uint32 boxesCount;
real cameraRange;
bool shadowEnabled;
std::atomic<bool> regenerate;

bool windowClose()
{
	engineStop();
	return false;
}

bool guiUpdate();

void updateBoxes(uint32 thrIndex, uint32 thrCount)
{
	uint64 time = engineControlTime();
	uint32 boxesCount = RenderComponent::component->group()->count();
	Entity *const *boxesEntities = RenderComponent::component->group()->array();

	uint32 myCount = boxesCount / thrCount;
	uint32 start = thrIndex * myCount;
	uint32 end = start + myCount;
	if (thrIndex == thrCount - 1)
		end = boxesCount;

	for (uint32 i = start; i != end; i++)
	{
		Entity *e = boxesEntities[i];
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position[1] = noise->evaluate(vec3(vec2(t.position[0], t.position[2]) * 0.15, time * 5e-8)) - 2;
	}
}

bool update()
{
	EntityManager *ents = engineEntities();

	if (regenerate)
	{
		regenerate = false;
		ents->destroy();

		{ // camera
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-30), degs(), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientLight = vec3(0.1);
			c.ambientDirectionalLight = vec3(0.2);
			c.effects = CameraEffectsFlags::CombinedPass;
			cameraCtrl->setEntity(e);
		}

		{ // light
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-20), degs(-110), degs());
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(0.9);
			if (shadowEnabled)
			{
				CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
				s.worldSize = vec3(20);
				s.resolution = 4096;
			}
		}

		// boxes
		uint32 side = numeric_cast<uint32>(sqrt(boxesCount));
#ifdef CAGE_DEBUG
		side /= 3;
#endif // CAGE_DEBUG

		for (uint32 y = 0; y < side; y++)
		{
			for (uint32 x = 0; x < side; x++)
			{
				Entity *e = ents->createAnonymous();
				CAGE_COMPONENT_ENGINE(Transform, t, e);
				t.scale = 0.15 * 0.49;
				t.position = vec3((x - side * 0.5) * 0.15, 0, (y - side * 0.5) * 0.15);
				CAGE_COMPONENT_ENGINE(Render, r, e);
				r.object = HashString("cage/mesh/fake.obj");
			}
		}
	}

	{ // camera
		Entity *e = ents->get(1);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.far = c.near + cameraRange * 50 + 1;
	}

	{ // update boxes
		updateThreads->run();
	}

	guiUpdate();

	return false;
}

bool guiInit()
{
	Gui *g = cage::engineGui();

	Entity *root = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(Scrollbars, sc, root);
	}

	Entity *panel = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(Parent, child, panel);
		child.parent = root->name();
		CAGE_COMPONENT_GUI(Panel, c, panel);
		CAGE_COMPONENT_GUI(LayoutTable, l, panel);
	}

	{ // boxes count
		Entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, child, lab);
			child.parent = panel->name();
			child.order = 1;
			CAGE_COMPONENT_GUI(Label, c, lab);
			CAGE_COMPONENT_GUI(Text, t, lab);
			t.value = "boxes: ";
		}
		Entity *con = g->entities()->create(1);
		{
			CAGE_COMPONENT_GUI(Parent, child, con);
			child.parent = panel->name();
			child.order = 2;
			CAGE_COMPONENT_GUI(Input, c, con);
			c.type = InputTypeEnum::Integer;
			c.min.i = 100;
			c.max.i = 100000;
			c.step.i = 100;
			c.value = "5000";
		}
	}

	{ // camera range
		Entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, child, lab);
			child.parent = panel->name();
			child.order = 3;
			CAGE_COMPONENT_GUI(Label, c, lab);
			CAGE_COMPONENT_GUI(Text, t, lab);
			t.value = "camera range: ";
		}
		Entity *con = g->entities()->create(2);
		{
			CAGE_COMPONENT_GUI(Parent, child, con);
			child.parent = panel->name();
			child.order = 4;
			CAGE_COMPONENT_GUI(SliderBar, c, con);
			c.value = 0.5;
		}
	}

	{ // shadow
		Entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, child, lab);
			child.parent = panel->name();
			child.order = 5;
			CAGE_COMPONENT_GUI(Label, c, lab);
			CAGE_COMPONENT_GUI(Text, t, lab);
			t.value = "enable shadow: ";
		}
		Entity *con = g->entities()->create(4);
		{
			CAGE_COMPONENT_GUI(Parent, child, con);
			child.parent = panel->name();
			child.order = 6;
			CAGE_COMPONENT_GUI(CheckBox, c, con);
			c.state = CheckBoxStateEnum::Checked;
		}
	}

	return false;
}

bool guiUpdate()
{
	{ // update boxes count
		Entity *e = cage::engineGui()->entities()->get(1);
		CAGE_COMPONENT_GUI(Input, c, e);
		if (c.valid && c.value.toUint32() != boxesCount)
		{
			boxesCount = c.value.toUint32();
			regenerate = true;
		}
	}

	{ // update camera range
		Entity *e = cage::engineGui()->entities()->get(2);
		CAGE_COMPONENT_GUI(SliderBar, c, e);
		cameraRange = c.value;
	}

	{ // update enable shadow
		Entity *e = cage::engineGui()->entities()->get(4);
		CAGE_COMPONENT_GUI(CheckBox, c, e);
		bool checked = c.state == CheckBoxStateEnum::Checked;
		if (checked != shadowEnabled)
		{
			shadowEnabled = checked;
			regenerate = true;
		}
	}

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
		GCHL_GENERATE((), guiInit, controlThread().initialize);
#undef GCHL_GENERATE
		EventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(engineWindow()->events.windowClose);

		engineWindow()->setMaximized();
		engineWindow()->title("performance");
		regenerate = true;

		cameraCtrl = newFpsCamera();
		cameraCtrl->movementSpeed = 0.1;
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;

		Holder<EngineProfiling> EngineProfiling = newEngineProfiling();

		updateThreads = newThreadPool();
		updateThreads->function.bind<&updateBoxes>();

		{
			NoiseFunctionCreateConfig cfg;
			cfg.type = NoiseTypeEnum::Cellular;
			cfg.operation = NoiseOperationEnum::Divide;
			noise = newNoiseFunction(cfg);
		}

		engineStart();
		engineFinalize();

		updateThreads.clear();

		cameraCtrl.clear();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
