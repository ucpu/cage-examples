#include <atomic>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/threadPool.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/cameraController.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

holder<cameraController> cameraCtrl;
holder<threadPool> updateThreads;
holder<noiseFunction> noise;
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
	uint64 time = currentControlTime();
	uint32 boxesCount = renderComponent::component->group()->count();
	entity *const *boxesEntities = renderComponent::component->group()->array();

	uint32 myCount = boxesCount / thrCount;
	uint32 start = thrIndex * myCount;
	uint32 end = start + myCount;
	if (thrIndex == thrCount - 1)
		end = boxesCount;

	for (uint32 i = start; i != end; i++)
	{
		entity *e = boxesEntities[i];
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.position[1] = noise->evaluate(vec3(vec2(t.position[0], t.position[2]) * 0.15, time * 5e-8)) - 2;
	}
}

bool update()
{
	entityManager *ents = entities();

	if (regenerate)
	{
		regenerate = false;
		ents->destroy();

		{ // camera
			entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-30), degs(), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.01;
			c.effects = cameraEffectsFlags::CombinedPass;
			cameraCtrl->setEntity(e);
		}

		{ // light
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-30), degs(-110), degs());
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(1, 1, 1) * 0.6;
			if (shadowEnabled)
			{
				CAGE_COMPONENT_ENGINE(shadowmap, s, e);
				s.worldSize = vec3(20, 20, 20);
				s.resolution = 2048;
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
				entity *e = ents->createAnonymous();
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.scale = 0.15 * 0.49;
				t.position = vec3((x - side * 0.5) * 0.15, 0, (y - side * 0.5) * 0.15);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage/mesh/fake.obj");
			}
		}
	}

	{ // camera
		entity *e = ents->get(1);
		CAGE_COMPONENT_ENGINE(camera, c, e);
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
	guiManager *g = cage::gui();

	entity *root = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(scrollbars, sc, root);
	}

	entity *panel = g->entities()->createUnique();
	{
		CAGE_COMPONENT_GUI(parent, child, panel);
		child.parent = root->name();
		CAGE_COMPONENT_GUI(panel, c, panel);
		CAGE_COMPONENT_GUI(layoutTable, l, panel);
	}

	{ // boxes count
		entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, child, lab);
			child.parent = panel->name();
			child.order = 1;
			CAGE_COMPONENT_GUI(label, c, lab);
			CAGE_COMPONENT_GUI(text, t, lab);
			t.value = "boxes: ";
		}
		entity *con = g->entities()->create(1);
		{
			CAGE_COMPONENT_GUI(parent, child, con);
			child.parent = panel->name();
			child.order = 2;
			CAGE_COMPONENT_GUI(input, c, con);
			c.type = inputTypeEnum::Integer;
			c.min.i = 100;
			c.max.i = 100000;
			c.step.i = 100;
			c.value = "5000";
		}
	}

	{ // camera range
		entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, child, lab);
			child.parent = panel->name();
			child.order = 3;
			CAGE_COMPONENT_GUI(label, c, lab);
			CAGE_COMPONENT_GUI(text, t, lab);
			t.value = "camera range: ";
		}
		entity *con = g->entities()->create(2);
		{
			CAGE_COMPONENT_GUI(parent, child, con);
			child.parent = panel->name();
			child.order = 4;
			CAGE_COMPONENT_GUI(sliderBar, c, con);
			c.value = 0.5;
		}
	}

	{ // shadow
		entity *lab = g->entities()->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, child, lab);
			child.parent = panel->name();
			child.order = 5;
			CAGE_COMPONENT_GUI(label, c, lab);
			CAGE_COMPONENT_GUI(text, t, lab);
			t.value = "enable shadow: ";
		}
		entity *con = g->entities()->create(4);
		{
			CAGE_COMPONENT_GUI(parent, child, con);
			child.parent = panel->name();
			child.order = 6;
			CAGE_COMPONENT_GUI(checkBox, c, con);
			c.state = checkBoxStateEnum::Checked;
		}
	}

	return false;
}

bool guiUpdate()
{
	{ // update boxes count
		entity *e = cage::gui()->entities()->get(1);
		CAGE_COMPONENT_GUI(input, c, e);
		if (c.valid && c.value.toUint32() != boxesCount)
		{
			boxesCount = c.value.toUint32();
			regenerate = true;
		}
	}

	{ // update camera range
		entity *e = cage::gui()->entities()->get(2);
		CAGE_COMPONENT_GUI(sliderBar, c, e);
		cameraRange = c.value;
	}

	{ // update enable shadow
		entity *e = cage::gui()->entities()->get(4);
		CAGE_COMPONENT_GUI(checkBox, c, e);
		bool checked = c.state == checkBoxStateEnum::Checked;
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
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		//configSetBool("cage-engine.engine.debugMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), guiInit, controlThread().initialize);
#undef GCHL_GENERATE
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(window()->events.windowClose);

		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("performance");
		regenerate = true;

		cameraCtrl = newCameraController();
		cameraCtrl->movementSpeed = 0.1;
		cameraCtrl->mouseButton = mouseButtonsFlags::Left;

		holder<engineProfiling> engineProfiling = newEngineProfiling();

		updateThreads = newThreadPool();
		updateThreads->function.bind<&updateBoxes>();

		{
			noiseFunctionCreateConfig cfg;
			cfg.type = noiseTypeEnum::Cellular;
			cfg.operation = noiseOperationEnum::Divide;
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
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
