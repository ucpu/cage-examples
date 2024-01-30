#include <cage-core/assetManager.h>
#include <cage-core/color.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/translucent/translucent.pack");
constexpr uint32 knotsCount = 40;
constexpr uint32 bulbsCount = 50;

Holder<NoiseFunction> bulbsNoise[3];
Vec3 bulbsVelocities[bulbsCount];

Vec3 bulbChange(const Vec3 &pos)
{
	Vec3 r;
	for (uint32 j = 0; j < 3; j++)
		r[j] = bulbsNoise[j]->evaluate(pos);
	return r;
}

void update()
{
	EntityManager *ents = engineEntities();

	{ // flying knot
		Entity *e = ents->get(6);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(0.5), Degs()) * t.orientation;
		t.position[1] += sin(Degs(engineControlTime() * 1e-5)) * 0.05;
	}

	// rotate cube, spheres, suzanne and bottle
	for (uint32 i = 10; i < 15; i++)
	{
		Entity *e = ents->get(i);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(i * 2 * 0.05), Degs()) * t.orientation;
	}

	// animated knot opacities
	for (uint32 i = 0; i < knotsCount * 2; i++)
	{
		Entity *e = ents->get(20 + i);
		const Real p = ((Real(i) + engineControlTime() * 3e-6) % knotsCount) / knotsCount;
		e->value<RenderComponent>().opacity = smootherstep(clamp(p * 2, 0, 1));
	}

	// moving light bulbs
	for (uint32 i = 0; i < bulbsCount; i++)
	{
		Entity *e = ents->get(100 + i);
		TransformComponent &t = e->value<TransformComponent>();
#if 1
		if (distance(t.position, Vec3(0, 3, 0)) > 10 || t.position[1] < -1)
			t.position = randomDirection3() * randomRange(1, 5) + Vec3(0, 3, 0);
		Vec3 &v = bulbsVelocities[i];
		v += bulbChange(t.position) * 0.1;
		v = normalize(v);
		t.position += v * 0.1;
#else
		Rads angle = Rads(Degs(i * 360.0 / bulbsCount)) + Degs(engineControlTime() * 5e-6);
		t.position = Vec3(sin(angle) * 5, 1, cos(angle) * 5);
#endif
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
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("translucent");

		// entities
		EntityManager *ents = engineEntities();
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
			e->value<ScreenSpaceEffectsComponent>();
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
			s.worldSize = Vec3(15);
		}
		{ // flash light
			Entity *e = ents->create(3);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-5, 4, 3);
			t.orientation = Quat(Degs(-40), Degs(-110), Degs());
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/bottle/other.obj?arrow");
			r.color = Vec3(1);
			r.intensity = 2;
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = Degs(60);
			l.spotExponent = 40;
			l.color = Vec3(1);
			l.intensity = 15;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 1024;
			s.worldSize = Vec3(3, 50, 0);
		}
		{ // floor
			Entity *e = ents->create(5);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Ground");
			e->value<TransformComponent>().position = Vec3(0, -0.2, 0);
		}
		{ // flying knot
			Entity *e = ents->create(6);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Knot");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(5, 2, -5);
			t.scale = 3;
		}
		{ // suzanne
			Entity *e = ents->create(10);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/suzanne.blend");
			e->value<TransformComponent>().position = Vec3(2, 1, 0);
		}
		{ // cube
			Entity *e = ents->create(11);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Cube");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-3, 1, -2);
			t.orientation = randomDirectionQuat();
		}
		{ // sphere with shadow
			Entity *e = ents->create(12);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Sphere;shadow");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(2, 1, 3.5);
			t.orientation = randomDirectionQuat();
		}
		{ // sphere no shadow
			Entity *e = ents->create(13);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Sphere");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-0.5, 1, 3.5);
			t.orientation = randomDirectionQuat();
		}
		{ // bottle
			Entity *e = ents->create(14);
			e->value<RenderComponent>().object = HashString("cage-tests/bottle/bottle.obj");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-1.1, -0.2, 5.8);
			t.scale = 0.6;
		}
		{ // plane translucent
			Entity *e = ents->create(15);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Plane");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-0.5, 1, -1);
			t.orientation = Quat(Degs(), randomAngle(), Degs());
		}
		{ // plane alphaClip
			Entity *e = ents->create(16);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Plane;alphaClip");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(4.5, 1, -1);
			t.orientation = Quat(Degs(), randomAngle(), Degs());
		}
		{ // plane billboard
			Entity *e = ents->create(17);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Plane;billboard");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-3.5, 1, 4);
			t.orientation = randomDirectionQuat();
		}
		// animated knots (transparent)
		for (uint32 i = 0; i < knotsCount; i++)
		{
			Entity *e = ents->create(20 + i);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Knot");
			TransformComponent &t = e->value<TransformComponent>();
			Rads angle = Degs(i * 360.0 / knotsCount);
			t.position = Vec3(sin(angle) * 8, 0.5, cos(angle) * 8);
			t.orientation = Quat(Degs(), angle, Degs());
			t.scale = 0.6;
		}
		// animated knots (fading)
		for (uint32 i = 0; i < knotsCount; i++)
		{
			Entity *e = ents->create(20 + knotsCount + i);
			e->value<RenderComponent>().object = HashString("cage-tests/translucent/shapes.blend?Knot;fade");
			TransformComponent &t = e->value<TransformComponent>();
			Rads angle = Degs(i * 360.0 / knotsCount);
			t.position = Vec3(sin(angle) * 10, 0.5, cos(angle) * 10);
			t.orientation = Quat(Degs(), angle, Degs());
			t.scale = 0.6;
		}
		// animated bulbs
		for (uint32 i = 0; i < bulbsCount; i++)
		{
			Entity *e = ents->create(100 + i);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/translucent/shapes.blend?Bulb");
			TransformComponent &t = e->value<TransformComponent>();
			t.scale = 0.1;
			t.position = randomDirection3() * randomRange(1, 5) + Vec3(0, 3, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Point;
			r.color = l.color = colorHsvToRgb(randomChance3() * Vec3(1, .5, .5) + Vec3(0, .5, .5));
			l.intensity = 20;
		}
		for (uint32 j = 0; j < 3; j++)
		{
			NoiseFunctionCreateConfig cfg;
			cfg.seed = j * 17 + 42 + randomRange(100, 1000);
			cfg.frequency *= 0.5;
			bulbsNoise[j] = newNoiseFunction(cfg);
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName);
		engineRun();
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
