#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/skybox/skybox.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
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
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("skybox");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 5, 0);
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // skybox
			Entity *e = ents->create(2);
			e->value<RenderComponent>().object = HashString("cage-tests/skybox/skybox.obj");
			e->value<TransformComponent>();
		}
		{ // sun
			Entity *e = ents->create(3);
			e->value<TransformComponent>().orientation = Quat(Degs(-50), Degs(123), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(50);
		}
		{ // floor
			Entity *e = ents->create(4);
			e->value<RenderComponent>().object = HashString("cage-tests/skybox/ground.obj");
			e->value<TransformComponent>().position = Vec3(0, -0.1, 0);
		}
		for (const auto &it : { Vec3(-10, 1, -10), Vec3(-10, 1, 10), Vec3(10, 1, -10), Vec3(10, 1, 10) })
		{ // boxes
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
			e->value<TransformComponent>().position = it;
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
