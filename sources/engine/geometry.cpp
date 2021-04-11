#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/geometry.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>

#include <cage-engine/window.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

void windowClose()
{
	engineStop();
}

void update()
{
	EntityManager *ents = engineEntities();

	const real t = engineControlTime() / double(1000000);
	const Cone shape = Cone(vec3(20 * sin(degs(30 * t)), 1, 0), vec3(0, 0, 1) * quat(degs(), degs(5 * t), degs()), 40, degs(20));

	// cubes
	for (uint32 i = 0; i < 10000; i++)
	{
		Entity *e = ents->get(100 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		const Aabb box = Aabb(vec3(-0.5 + t.position), vec3(0.5 + t.position));
		t.scale = intersects(Sphere(box), shape) ? 0.5 : 0.1;
	}
}

void init()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(0, 50, 70);
		t.orientation = quat(degs(-40), degs(), degs());
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.near = 0.1;
		c.far = 1000;
		c.effects = CameraEffectsFlags::Default;
		c.ambientColor = vec3(1);
		c.ambientIntensity = 0.5;
		c.ambientDirectionalColor = vec3(1);
		c.ambientDirectionalIntensity = 0.5;
	}

	// cubes
	for (uint32 i = 0; i < 10000; i++)
	{
		Entity *e = ents->create(100 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(i / 100, 0, i % 100) + vec3(-50, 1, -50);
		t.scale = 0;
		CAGE_COMPONENT_ENGINE(Render, r, e);
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

		EventListener<void()> windowCloseListener;
		windowCloseListener.attach(engineWindow()->events.windowClose);
		windowCloseListener.bind<&windowClose>();

		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("geometry");

		init();

		Holder<FpsCamera> fpsCamera = newFpsCamera(engineEntities()->get(1));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
