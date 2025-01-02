#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/enumerate.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/bezier/bezier.pack");

void update()
{
	EntityManager *ents = engineEntities();
	for (uint32 i : { 50, 60 })
	{
		const Vec3 p1 = ents->get(i + 1)->value<TransformComponent>().position;
		const Vec3 p2 = ents->get(i + 2)->value<TransformComponent>().position;
		const Vec3 p3 = ents->get(i + 3)->value<TransformComponent>().position;
		const Vec3 p4 = ents->get(i + 4)->value<TransformComponent>().position;
		Entity *e = ents->get(i);
		e->value<TransformComponent>().position = p1;
		e->value<TransformComponent>().orientation = Quat(p4 - p1, Vec3(0, 1, 0));
		e->value<TransformComponent>().scale = distance(p4, p1);
		e->value<ShaderDataComponent>().data[0] = Vec4(p2, 0);
		e->value<ShaderDataComponent>().data[1] = Vec4(p3, 0);
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("bezier");

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
			c.ambientIntensity = 0.2;
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
			s.directionalWorldSize = 15;
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

		// control points A
		static constexpr Vec3 pointsA[4] = { Vec3(-3, 1, -1), Vec3(-1, 4, -1), Vec3(3, 1, -4), Vec3(7, 3, 1) };
		for (const auto it : enumerate(pointsA))
		{
			Entity *e = ents->create(51 + it.index);
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
			e->value<TransformComponent>().position = *it;
			e->value<TransformComponent>().scale = 0.1;
		}

		{ // bezier A
			Entity *e = ents->create(50);
			e->value<RenderComponent>().object = HashString("cage-tests/bezier/bezier.obj");
			e->value<TransformComponent>();
		}

		// control points B
		static constexpr Vec3 pointsB[4] = { Vec3(-8, 1, -7), Vec3(-8, 5, -7), Vec3(-9, 8, -2), Vec3(-9, 4, -2) };
		for (const auto it : enumerate(pointsB))
		{
			Entity *e = ents->create(61 + it.index);
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
			e->value<TransformComponent>().position = *it;
			e->value<TransformComponent>().scale = 0.1;
			e->value<TransformComponent>().orientation = Quat(Degs(45), Degs(45), Degs(45));
		}

		{ // bezier B
			Entity *e = ents->create(60);
			e->value<RenderComponent>().object = HashString("cage-tests/bezier/bezier.obj");
			e->value<TransformComponent>();
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
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
