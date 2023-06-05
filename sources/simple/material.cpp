#include <cage-core/assetManager.h>
#include <cage-core/color.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/material/material.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	EntityManager *ents = engineEntities();

	for (uint32 i = 10; i < 13; i++)
	{ // rotate the material plane
		Entity *e = ents->get(i);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(1), Degs()) * t.orientation;
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
		const auto closeListener = engineWindow()->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("material");

		// entities
		EntityManager *ents = engineEntities();
		{ // skybox
			Entity *e = ents->create(1);
			e->value<RenderComponent>().object = HashString("cage-tests/skybox/skybox.obj");
			e->value<TransformComponent>().orientation = Quat(Degs(180), Degs(), Degs());
		}
		{ // sun
			Entity *e = ents->create(2);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			e->value<TransformComponent>().orientation = Quat(Degs(-30), Degs(180), Degs());
		}
		{ // camera
			Entity *e = ents->create(3);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.01;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>().effects &= ~ScreenSpaceEffectsFlags::Bloom;
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 3, 10);
			t.orientation = Quat(Degs(-10), Degs(), Degs());
		}

		{ // a
			Entity *e = ents->create(10);
			e->value<RenderComponent>().object = HashString("cage-tests/material/plane.obj;a");
			e->value<TransformComponent>().position = Vec3(-10, 0.001, 0);
			e->value<TransformComponent>().scale = 4;
		}
		{ // b
			Entity *e = ents->create(11);
			e->value<RenderComponent>().object = HashString("cage-tests/material/plane.obj;b");
			e->value<TransformComponent>().position = Vec3();
			e->value<TransformComponent>().scale = 4;
		}
		{ // c
			Entity *e = ents->create(12);
			e->value<RenderComponent>().object = HashString("cage-tests/material/plane.obj;c");
			e->value<TransformComponent>().position = Vec3(10, 0.001, 0);
			e->value<TransformComponent>().scale = 4;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(3));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName);
		engineRun();
		engineAssets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
