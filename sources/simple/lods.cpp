#include <vector>

#include <cage-core/assetManager.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/lods/lods.pack");

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
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("levels of details");

		// entities
		EntityManager *ents = engineEntities();
		{ // cube
			Entity *e = ents->create(1);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/lods/cube.object");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-8, 0, 0);
		}
		{ // sphere
			Entity *e = ents->create(2);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/lods/sphere.object");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(8, 0, 0);
		}
		{ // floor
			Entity *e = ents->create(3);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/lods/floor.object");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, -5, 0);
		}
		{ // sun
			Entity *e = ents->create(9);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-60), Degs(20), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 1024;
			s.directionalWorldSize = 20;
		}
		{ // camera
			Entity *e = ents->create(10);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 3, 30);
			t.orientation = Quat(Degs(-10), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.5;
			c.near = 0.1;
			c.far = 200;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(10));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 1;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->load(AssetsName);
		engineRun();
		engineAssets()->unload(AssetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
