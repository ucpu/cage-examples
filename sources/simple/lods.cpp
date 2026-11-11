#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
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
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("levels of details");

		// entities
		EntityManager *ents = engineEntities();
		{ // cube simple
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/cubeSimple.object");
			e->value<TransformComponent>().position = Vec3(-45, 0, 0);
		}
		{ // cube 1
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/cube.object");
			e->value<TransformComponent>().position = Vec3(-30, 0, 0);
		}
		{ // cube 2
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/cube2.object");
			e->value<TransformComponent>().position = Vec3(-15, 0, 0);
		}
		{ // sphere ref
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/sphereRef.object");
			e->value<TransformComponent>().position = Vec3(0, 0, 0);
		}
		{ // sphere 2
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/sphere2.object");
			e->value<TransformComponent>().position = Vec3(15, 0, 0);
		}
		{ // sphere 1
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/sphere.object");
			e->value<TransformComponent>().position = Vec3(30, 0, 0);
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<ModelComponent>() = HashString("cage-tests/lods/floor.object");
			e->value<TransformComponent>().position = Vec3(0, -5, 0);
		}
		{ // sun
			Entity *e = ents->create(9);
			e->value<TransformComponent>().orientation = Quat(Degs(-60), Degs(20), Degs());
			e->value<LightComponent>().lightType = LightTypeEnum::Directional;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 1024;
			s.cascadesPaddingDistance = 20;
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
			c.far = 500;
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
