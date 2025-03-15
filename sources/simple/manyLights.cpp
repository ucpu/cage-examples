#include <cage-core/assetsManager.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/translucent/translucent.pack");

struct MovingComponent
{
	Vec3 velocity;
};

struct OrbitingComponent
{
	Vec3 origin;
	Real dist;
	Rads pitch, yaw;
	Rads yawSpeed;

	Transform make(uint64 time) const
	{
		Transform t;
		t.orientation = Quat(pitch, yaw + yawSpeed * time, {});
		t.position = origin + t.orientation * Vec3(0, 0, dist);
		t.scale = 0.1;
		return t;
	}
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

	const uint64 time = engineControlTime();
	entitiesVisitor([time](TransformComponent &t, const OrbitingComponent &orb) { t = orb.make(time); }, engineEntities(), false);
}

void initializeGui()
{
	Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
	auto _1 = g->alignment(Vec2());
	auto _2 = g->panel();
	auto _3 = g->row();
	g->label().text("lights limit: ");
	g->horizontalSliderBar(CameraComponent().maxLights, 0, 200)
		.event(inputFilter(
			[](input::GuiValue in)
			{
				Entity *e = engineEntities()->get(1);
				e->value<CameraComponent>().maxLights = numeric_cast<uint32>(in.entity->value<GuiSliderBarComponent>().value);
			}));
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
		engineWindow()->title("many lights");
		initializeGui();

		// entities
		EntityManager *ents = engineEntities();
		ents->defineComponent(MovingComponent());
		ents->defineComponent(OrbitingComponent());
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
			ScreenSpaceEffectsComponent &s = e->value<ScreenSpaceEffectsComponent>();
			s.effects &= ~ScreenSpaceEffectsFlags::Bloom;
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			t.position = Vec3(0, 5, 0);
			e->value<LightComponent>().lightType = LightTypeEnum::Directional;
			e->value<ColorComponent>().color = Vec3(1, 1, 0.5);
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 4096;
			s.directionalWorldSize = 20;
		}
		{ // green floor
			Entity *e = ents->create(5);
			e->value<ModelComponent>().model = HashString("cage-tests/translucent/shapes.blend?Ground");
			e->value<TransformComponent>().position = Vec3(0, 0, 0);
		}
		{ // gray floor
			Entity *e = ents->create(6);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/other.obj?plane");
			e->value<TransformComponent>().position = Vec3(0, 0, 0);
		}
		{ // bottle
			Entity *e = ents->create(7);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/bottle.obj");
			e->value<TransformComponent>().position += Vec3(1, 0, 0);
			e->value<TransformComponent>().scale = 0.3;
		}
		// spot lights
		for (uint32 i = 0; i < 3; i++)
		{
			Entity *e = ents->createUnique();
			e->value<TransformComponent>();
			OrbitingComponent &orb = e->value<OrbitingComponent>();
			orb.origin = (randomChance3() * 2 - 1) * (i * 3 + 1) * Vec3(1, 0, 1);
			orb.dist = randomRange(2, 5);
			orb.pitch = Degs(randomRange(-50, -40));
			orb.yaw = randomAngle();
			orb.yawSpeed = Degs(20 * 1e-6);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/other.obj?arrow");
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = Degs(60);
			l.spotExponent = 10;
			l.minDistance = 1;
			l.maxDistance = 100;
			e->value<ColorComponent>() = { Vec3(i == 0, i == 1, i == 2), 3 };
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 4096;
			s.directionalWorldSize = 500;
		}
		// point lights
		for (uint32 i = 0; i < 500; i++)
		{
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = randomDirection3() * 8 * Vec3(1, 0, 1) + Vec3(1, 0.2, 1);
			e->value<TransformComponent>().scale = 0.03;
			e->value<MovingComponent>().velocity = randomDirection3() * 0.02 * Vec3(1, 0, 1);
			e->value<ModelComponent>().model = HashString("cage-tests/translucent/shapes.blend?Bulb");
			e->value<LightComponent>();
			e->value<ColorComponent>() = { randomChance3() * 0.5 + 0.5, randomRange(0.5, 2.0) };
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
