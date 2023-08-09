#include <atomic>

#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/string.h>
#include <cage-core/tasks.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

Holder<FpsCamera> cameraCtrl;
Holder<NoiseFunction> noise;
uint32 boxesCount;
Real cameraRange;
bool shadowEnabled;
std::atomic<bool> regenerate;

void windowClose(InputWindow)
{
	engineStop();
}

void guiUpdate();

struct BoxUpdater
{
	uint64 time = engineControlTime();
	uint32 boxesCount = engineEntities()->component<RenderComponent>()->count();
	PointerRange<Entity *const> boxesEntities = engineEntities()->component<RenderComponent>()->entities();
	EntityComponent *transformComponent = engineEntities()->component<TransformComponent>();

	void operator()(uint32 grp) const
	{
		const auto r = tasksSplit(grp, Groups, boxesCount);
		for (uint32 i = r.first; i < r.second; i++)
		{
			Entity *e = boxesEntities[i];
			TransformComponent &t = e->value<TransformComponent>(transformComponent);
			t.position[1] = noise->evaluate(Vec3(Vec2(t.position[0], t.position[2]) * 0.15, time * 5e-8)) - 2;
		}
	}

	static const uint32 Groups = 64;
};

void update()
{
	EntityManager *ents = engineEntities();

	if (regenerate)
	{
		regenerate = false;
		ents->destroy();

		{ // camera
			Entity *e = ents->create(1);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-30), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
			e->value<ScreenSpaceEffectsComponent>();
			cameraCtrl->setEntity(e);
		}

		{ // light
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-20), Degs(-110), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(0.9);
			if (shadowEnabled)
			{
				ShadowmapComponent &s = e->value<ShadowmapComponent>();
				s.worldSize = Vec3(20);
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
				TransformComponent &t = e->value<TransformComponent>();
				t.scale = 0.15 * 0.49;
				t.position = Vec3((x - side * 0.5) * 0.15, 0, (y - side * 0.5) * 0.15);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage/model/fake.obj");
			}
		}
	}

	{ // camera
		Entity *e = ents->get(1);
		CameraComponent &c = e->value<CameraComponent>();
		c.far = c.near + cameraRange * 50 + 1;
	}

	{ // update boxes
		BoxUpdater updater;
		tasksRunBlocking<BoxUpdater>("update boxes", updater, BoxUpdater::Groups);
	}

	guiUpdate();
}

void guiInit()
{
	GuiManager *g = cage::engineGuiManager();

	Entity *root = g->entities()->createUnique();
	{
		root->value<GuiLayoutScrollbarsComponent>();
		root->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0);
	}

	Entity *panel = g->entities()->createUnique();
	{
		GuiParentComponent &child = panel->value<GuiParentComponent>();
		child.parent = root->name();
		panel->value<GuiPanelComponent>();
		panel->value<GuiLayoutTableComponent>();
	}

	{ // boxes count
		Entity *lab = g->entities()->createUnique();
		{
			GuiParentComponent &child = lab->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 1;
			lab->value<GuiLabelComponent>();
			GuiTextComponent &t = lab->value<GuiTextComponent>();
			t.value = "boxes: ";
		}
		Entity *con = g->entities()->create(1);
		{
			GuiParentComponent &child = con->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 2;
			GuiInputComponent &c = con->value<GuiInputComponent>();
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
			GuiParentComponent &child = lab->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 3;
			lab->value<GuiLabelComponent>();
			GuiTextComponent &t = lab->value<GuiTextComponent>();
			t.value = "camera range: ";
		}
		Entity *con = g->entities()->create(2);
		{
			GuiParentComponent &child = con->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 4;
			GuiSliderBarComponent &c = con->value<GuiSliderBarComponent>();
			c.value = 0.5;
		}
	}

	{ // shadow
		Entity *lab = g->entities()->createUnique();
		{
			GuiParentComponent &child = lab->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 5;
			lab->value<GuiLabelComponent>();
			GuiTextComponent &t = lab->value<GuiTextComponent>();
			t.value = "enable shadow: ";
		}
		Entity *con = g->entities()->create(4);
		{
			GuiParentComponent &child = con->value<GuiParentComponent>();
			child.parent = panel->name();
			child.order = 6;
			GuiCheckBoxComponent &c = con->value<GuiCheckBoxComponent>();
			c.state = CheckBoxStateEnum::Checked;
		}
	}
}

void guiUpdate()
{
	{ // update boxes count
		Entity *e = cage::engineGuiEntities()->get(1);
		GuiInputComponent &c = e->value<GuiInputComponent>();
		if (c.valid && toUint32(c.value) != boxesCount)
		{
			boxesCount = toUint32(c.value);
			regenerate = true;
		}
	}

	{ // update camera range
		Entity *e = cage::engineGuiEntities()->get(2);
		GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
		cameraRange = c.value;
	}

	{ // update enable shadow
		Entity *e = cage::engineGuiEntities()->get(4);
		GuiCheckBoxComponent &c = e->value<GuiCheckBoxComponent>();
		bool checked = c.state == CheckBoxStateEnum::Checked;
		if (checked != shadowEnabled)
		{
			shadowEnabled = checked;
			regenerate = true;
		}
	}
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
		const auto updateListener = controlThread().update.listen(&update);
		const auto initListener = controlThread().initialize.listen(&guiInit);
		const auto closeListener = engineWindow()->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));

		engineWindow()->setMaximized();
		engineWindow()->title("performance");
		regenerate = true;

		cameraCtrl = newFpsCamera();
		cameraCtrl->movementSpeed = 0.1;
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;

		Holder<StatisticsGui> statistics = newStatisticsGui();

		{
			NoiseFunctionCreateConfig cfg;
			cfg.type = NoiseTypeEnum::Cellular;
			cfg.operation = NoiseOperationEnum::Divide;
			noise = newNoiseFunction(cfg);
		}

		engineRun();
		engineFinalize();

		cameraCtrl.clear();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
