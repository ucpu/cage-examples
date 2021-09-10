#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>

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

	{ // rotate the material plane
		Entity *e = ents->get(1);
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
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("material");

		// entities
		EntityManager *ents = engineEntities();
		{ // material
			Entity *e = ents->create(1);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/material/plane.obj");
			r.color = Vec3(255, 226, 155) / 255;
			TransformComponent &t = e->value<TransformComponent>();
			t.scale = 5;
		}
		{ // sun
			Entity *e = ents->create(2);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-30), Degs(180), Degs());
		}
		{ // camera
			Entity *e = ents->create(3);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.01;
			c.near = 0.1;
			c.far = 100;
			c.effects = CameraEffectsFlags::Default;
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 3, 10);
			t.orientation = Quat(Degs(-10), Degs(), Degs());
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
