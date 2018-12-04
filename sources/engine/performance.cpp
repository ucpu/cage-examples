#include <atomic>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-core/noise.h>
#include <cage-core/threadPool.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/cameraController.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

holder<cameraControllerClass> cameraController;
holder<threadPoolClass> updateThreads;
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
	entityClass *const *boxesEntities = renderComponent::component->group()->array();

	uint32 myCount = boxesCount / thrCount;
	uint32 start = thrIndex * myCount;
	uint32 end = start + myCount;
	if (thrIndex == thrCount - 1)
		end = boxesCount;

	for (uint32 i = start; i != end; i++)
	{
		entityClass *e = boxesEntities[i];
		ENGINE_GET_COMPONENT(transform, t, e);
		t.position[1] = noiseClouds(42, vec3(vec2(t.position[0], t.position[2]) * 0.15, time * 5e-8)) * 2 - 3;
	}
}

bool update()
{
	entityManagerClass *ents = entities();

	if (regenerate)
	{
		regenerate = false;
		ents->destroy();

		{ // camera
			entityClass *e = ents->create(1);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-30), degs(), degs());
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.1;
			cameraController->setEntity(e);
		}

		{ // light
			entityClass *e = ents->create(2);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-30), degs(-110), degs());
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(1, 1, 1) * 0.9;
			if (shadowEnabled)
			{
				ENGINE_GET_COMPONENT(shadowmap, s, e);
				s.worldSize = vec3(20, 20, 20);
				s.resolution = 1024;
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
				entityClass *e = ents->createAnonymous();
				ENGINE_GET_COMPONENT(transform, t, e);
				t.scale = 0.15 * 0.49;
				t.position = vec3((x - side * 0.5) * 0.15, 0, (y - side * 0.5) * 0.15);
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = 1;
			}
		}
	}

	{ // camera
		entityClass *e = ents->get(1);
		ENGINE_GET_COMPONENT(camera, c, e);
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
	guiClass *g = cage::gui();

	entityClass *root = g->entities()->createUnique();
	{
		GUI_GET_COMPONENT(scrollbars, sc, root);
	}

	entityClass *panel = g->entities()->createUnique();
	{
		GUI_GET_COMPONENT(parent, child, panel);
		child.parent = root->name();
		GUI_GET_COMPONENT(panel, c, panel);
		GUI_GET_COMPONENT(layoutTable, l, panel);
	}

	{ // boxes count
		entityClass *lab = g->entities()->createUnique();
		{
			GUI_GET_COMPONENT(parent, child, lab);
			child.parent = panel->name();
			child.order = 1;
			GUI_GET_COMPONENT(label, c, lab);
			GUI_GET_COMPONENT(text, t, lab);
			t.value = "boxes: ";
		}
		entityClass *con = g->entities()->create(1);
		{
			GUI_GET_COMPONENT(parent, child, con);
			child.parent = panel->name();
			child.order = 2;
			GUI_GET_COMPONENT(input, c, con);
			c.type = inputTypeEnum::Integer;
			c.min.i = 100;
			c.max.i = 100000;
			c.step.i = 100;
			c.value = "5000";
		}
	}

	{ // camera range
		entityClass *lab = g->entities()->createUnique();
		{
			GUI_GET_COMPONENT(parent, child, lab);
			child.parent = panel->name();
			child.order = 3;
			GUI_GET_COMPONENT(label, c, lab);
			GUI_GET_COMPONENT(text, t, lab);
			t.value = "camera range: ";
		}
		entityClass *con = g->entities()->create(2);
		{
			GUI_GET_COMPONENT(parent, child, con);
			child.parent = panel->name();
			child.order = 4;
			GUI_GET_COMPONENT(sliderBar, c, con);
			c.value = 0.5;
		}
	}

	{ // shadow
		entityClass *lab = g->entities()->createUnique();
		{
			GUI_GET_COMPONENT(parent, child, lab);
			child.parent = panel->name();
			child.order = 5;
			GUI_GET_COMPONENT(label, c, lab);
			GUI_GET_COMPONENT(text, t, lab);
			t.value = "enable shadow: ";
		}
		entityClass *con = g->entities()->create(4);
		{
			GUI_GET_COMPONENT(parent, child, con);
			child.parent = panel->name();
			child.order = 6;
			GUI_GET_COMPONENT(checkBox, c, con);
			c.state = checkBoxStateEnum::Checked;
		}
	}

	return false;
}

bool guiUpdate()
{
	{ // update boxes count
		entityClass *e = cage::gui()->entities()->get(1);
		GUI_GET_COMPONENT(input, c, e);
		if (c.valid && c.value.toUint32() != boxesCount)
		{
			boxesCount = c.value.toUint32();
			regenerate = true;
		}
	}

	{ // update camera range
		entityClass *e = cage::gui()->entities()->get(2);
		GUI_GET_COMPONENT(sliderBar, c, e);
		cameraRange = c.value;
	}

	{ // update enable shadow
		entityClass *e = cage::gui()->entities()->get(4);
		GUI_GET_COMPONENT(checkBox, c, e);
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
		holder <loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		configSetBool("cage-client.engine.renderMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), guiInit, controlThread().initialize);
#undef GCHL_GENERATE
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(window()->events.windowClose);

		window()->modeSetWindowed(windowFlags::Border | windowFlags::Resizeable);
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test cpu performance (automatic instancing)");
		regenerate = true;

		cameraController = newCameraController();
		cameraController->movementSpeed = 0.1;
		cameraController->mouseButton = mouseButtonsFlags::Left;

		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

		updateThreads = newThreadPool();
		updateThreads->function.bind<&updateBoxes>();

		engineStart();
		engineFinalize();

		updateThreads.clear();

		cameraController.clear();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
