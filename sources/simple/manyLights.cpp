#include <cage-core/assetManager.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/translucent/translucent.pack");
constexpr uint32 lightsCount = 500;

struct MovingComponent
{
	Vec3 velocity;
};

void update()
{
	entitiesVisitor(
		[](TransformComponent &t, MovingComponent &mv)
		{
			t.position += mv.velocity;
			if (length(t.position) > 10)
				mv.velocity *= -1;
		},
		engineEntities(), false);
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
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("many lights");

		// entities
		EntityManager *ents = engineEntities();
		ents->defineComponent(MovingComponent());
		{ // camera
			Entity *e = ents->create(1);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(8, 6, 10);
			t.orientation = Quat(Degs(-30), Degs(40), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			t.position = Vec3(0, 5, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1, 1, 0.5);
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.directionalWorldSize = 15;
		}
		{ // floor
			Entity *e = ents->create(5);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Ground");
			e->value<TransformComponent>().position = Vec3(0, -0.2, 0);
		}
		// lights
		for (uint32 i = 0; i < lightsCount; i++)
		{
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = (randomChance3() * 2 - 1) * 5 * Vec3(1, 0, 1);
			e->value<TransformComponent>().scale = 0.03;
			e->value<MovingComponent>().velocity = randomDirection3() * 0.02 * Vec3(1, 0, 1);
			e->value<LightComponent>().color = randomChance3() * 0.5 + 0.5;
			e->value<LightComponent>().intensity = randomRange(0.01, 3.0);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Bulb");
			e->value<RenderComponent>().color = e->value<LightComponent>().color;
			e->value<RenderComponent>().intensity = e->value<LightComponent>().intensity;
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
