#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/string.h>
#include <cage-core/tasks.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

Holder<NoiseFunction> noise = []()
{
	NoiseFunctionCreateConfig cfg;
	cfg.type = NoiseTypeEnum::Cellular;
	cfg.operation = NoiseOperationEnum::Divide;
	return newNoiseFunction(cfg);
}();

Holder<FpsCamera> cameraCtrl;
uint32 boxesCount = 5000;
Real cameraRange = 0.5;
bool shadowEnabled = true;

struct BoxUpdater
{
	const uint64 time = engineControlTime();
	EntityComponent *const transformComponent = engineEntities()->component<TransformComponent>();

	void run()
	{
		PointerRange<Entity *const> boxesEntities = engineEntities()->component<RenderComponent>()->entities();
		tasksRunBlocking<Entity *const, 1024>(
			"update boxes",
			[&](Entity *e)
			{
				TransformComponent &t = e->value<TransformComponent>(transformComponent);
				t.position[1] = noise->evaluate(Vec3(Vec2(t.position[0], t.position[2]) * 0.15, time * 5e-8)) - 2;
			},
			boxesEntities);
	}
};

void generate()
{
	EntityManager *ents = engineEntities();
	ents->destroy();

	{ // camera
		Entity *e = ents->create(1);
		e->value<TransformComponent>().orientation = Quat(Degs(-30), Degs(), Degs());
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 0.2;
		e->value<ScreenSpaceEffectsComponent>();
		cameraCtrl->setEntity(e);
	}

	{ // light
		Entity *e = ents->create(2);
		e->value<TransformComponent>().orientation = Quat(Degs(-20), Degs(-110), Degs());
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
			t.position = Vec3((x - side * 0.5) * 0.15, -2, (y - side * 0.5) * 0.15);
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
		}
	}

	{ // update boxes
		BoxUpdater updater;
		updater.run();
	}
}

const auto updateListener = controlThread().update.listen(
	[]()
	{
		{ // camera
			Entity *e = engineEntities()->get(1);
			CameraComponent &c = e->value<CameraComponent>();
			c.far = c.near + cameraRange * 50 + 1;
		}

		{ // update boxes
			BoxUpdater updater;
			updater.run();
		}
	});

const auto initListener = controlThread().initialize.listen(
	[]()
	{
		Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->alignment(Vec2(0));
		auto _2 = g->panel();
		auto _3 = g->verticalTable(2);

		{
			g->label().text("boxes: ");
			g->input(boxesCount, 100, 100000, 100)
				.event(inputFilter(
					[](input::GuiValue in)
					{
						auto &i = in.entity->value<GuiInputComponent>();
						if (i.valid)
						{
							boxesCount = toUint32(i.value);
							generate();
						}
						return true;
					}));
		}

		{
			g->label().text("camera range: ");
			g->horizontalSliderBar(cameraRange)
				.event(inputFilter(
					[](input::GuiValue in)
					{
						cameraRange = in.entity->value<GuiSliderBarComponent>().value;
						return true;
					}));
		}

		{
			g->label().text("enable shadow: ");
			g->checkBox(shadowEnabled)
				.event(inputFilter(
					[](input::GuiValue in)
					{
						shadowEnabled = in.entity->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked;
						generate();
						return true;
					}));
		}
	});

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(EngineCreateConfig());

		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));
		engineWindow()->setMaximized();
		engineWindow()->title("performance");

		cameraCtrl = newFpsCamera();
		cameraCtrl->movementSpeed = 0.1;
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;

		Holder<StatisticsGui> statistics = newStatisticsGui();

		generate();
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
