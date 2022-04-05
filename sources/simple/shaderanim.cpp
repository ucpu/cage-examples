#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/noiseFunction.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/shaderanim/shaderanim.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{}

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
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("maze");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 5, 10);
			e->value<TransformComponent>().orientation = Quat(Degs(-20), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.1;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->createUnique();
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(42), Degs());
			t.position = Vec3(0, 2, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 1;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(15);
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/skeletons/floor/floor.obj");
			e->value<TransformComponent>().position = Vec3();
		}
		{ // skybox
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/screamers/skybox.obj");
			e->value<TransformComponent>();
		}
		{ // tower
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/shaderanim/tower-wind.object");
			e->value<TransformComponent>().position = Vec3();
		}
		{ // tree 1
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/shaderanim/tree-1-wind.object");
			e->value<TransformComponent>().position = Vec3(-4, 0, 0);
		}
		{ // tree 2
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/shaderanim/tree-2-wind.object");
			e->value<TransformComponent>().position = Vec3(4, 0, 0);
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName);
		engineStart();
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
