#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/geometry.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/camera.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

bool autoCubes = true;

void windowClose(InputWindow)
{
	engineStop();
}

Frustum cameraFrustum()
{
	Entity *camE = engineEntities()->get(1);
	TransformComponent &camT = camE->value<TransformComponent>();
	CameraComponent &camC = camE->value<CameraComponent>();
	const Vec2i resolution = engineWindow()->resolution();
	const Frustum frustum = Frustum(camT, perspectiveProjection(camC.camera.perspectiveFov, Real(resolution[0]) / resolution[1], camC.near, camC.far));
	return frustum;
}

void keyPress(InputKey in)
{
	EntityManager *ents = engineEntities();

	if (in.key == 32)
	{
		autoCubes = false;

		const Frustum frustum = cameraFrustum();

		// cubes
		for (uint32 i = 0; i < 10000; i++)
		{
			Entity *e = ents->get(100 + i);
			TransformComponent &t = e->value<TransformComponent>();
			const Aabb box = Aabb(Vec3(-0.5 + t.position), Vec3(0.5 + t.position));
			t.scale = intersects(box, frustum) ? 0.5 : 0.1;
		}
	}
}

void update()
{
	if (!autoCubes)
		return;

	EntityManager *ents = engineEntities();

	const Real t = engineControlTime() / double(1000000);
	const Cone shape = Cone(Vec3(20 * sin(Degs(30 * t)), 1, 0), Vec3(0, 0, 1) * Quat(Degs(), Degs(5 * t), Degs()), 40, Degs(20));
	//const Frustum shape = Frustum(transform(vec3(20 * sin(degs(30 * t)), 1, 0), quat(degs(180), degs(5 * t), degs())), perspectiveProjection(degs(60), 1, 10, 40));
	//const auto shape = cameraFrustum();

	// cubes
	for (uint32 i = 0; i < 10000; i++)
	{
		Entity *e = ents->get(100 + i);
		TransformComponent &t = e->value<TransformComponent>();
		const Aabb box = Aabb(Vec3(-0.5 + t.position), Vec3(0.5 + t.position));
		t.scale = intersects(Sphere(box), shape) ? 0.5 : 0.1;
	}
}

void init()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(0, 50, 70);
		t.orientation = Quat(Degs(-40), Degs(), Degs());
		CameraComponent &c = e->value<CameraComponent>();
		c.near = 0.1;
		c.far = 1000;
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 1;
		e->value<ScreenSpaceEffectsComponent>();
	}

	// cubes
	for (uint32 i = 0; i < 10000; i++)
	{
		Entity *e = ents->create(100 + i);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(i / 100, 0, i % 100) + Vec3(-50, 1, -50);
		t.scale = 0;
		RenderComponent &r = e->value<RenderComponent>();
		r.object = HashString("cage/model/fake.obj");
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
		const auto keyPressListener = engineWindow()->events.listen(inputListener<InputClassEnum::KeyPress, InputKey>(&keyPress));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("geometry");

		init();

		Holder<FpsCamera> fpsCamera = newFpsCamera(engineEntities()->get(1));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		Holder<StatisticsGui> statistics = newStatisticsGui();

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
