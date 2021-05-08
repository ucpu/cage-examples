#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/material/material.pack");

void windowClose()
{
	engineStop();
}

void update()
{
	EntityManager *ents = engineEntities();

	{ // rotate the material plane
		Entity *e = ents->get(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(), degs(1), degs()) * t.orientation;
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
		EventListener<void()> closeListener;
		closeListener.attach(engineWindow()->events.windowClose);
		closeListener.bind<&windowClose>();
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("material");

		// entities
		EntityManager *ents = engineEntities();
		{ // material
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/material/plane.obj");
			r.color = vec3(255, 226, 155) / 255;
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.scale = 5;
		}
		{ // sun
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(1);
			l.intensity = 3;
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-30), degs(180), degs());
		}
		{ // camera
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.01;
			c.near = 0.1;
			c.far = 100;
			c.effects = CameraEffectsFlags::Default;
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 3, 10);
			t.orientation = quat(degs(-10), degs(), degs());
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(3));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;

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
