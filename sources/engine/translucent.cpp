#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/noiseFunction.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = HashString("cage-tests/translucent/translucent.pack");
static const uint32 knotsCount = 40;
static const uint32 bulbsCount = 5;

Holder<NoiseFunction> bulbsNoise[3];
vec3 bulbsVelocities[bulbsCount];

vec3 bulbChange(const vec3 &pos)
{
	vec3 r;
	for (uint32 j = 0; j < 3; j++)
		r[j] = bulbsNoise[j]->evaluate(pos);
	return r;
}

bool windowClose()
{
	engineStop();
	return false;
}

bool update()
{
	EntityManager *ents = engineEntities();

	{ // flying knot
		Entity *e = ents->get(6);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(), degs(0.5), degs()) * t.orientation;
		t.position[1] += sin(degs(engineControlTime() * 1e-5)) * 0.05;
	}

	// rotate cube, sphere and suzanne
	for (uint32 i = 10; i < 13; i++)
	{
		Entity *e = ents->get(i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(), degs(i * 2 * 0.05), degs()) * t.orientation;
	}

	// animated knot opacities
	for (uint32 i = 0; i < knotsCount; i++)
	{
		Entity *e = ents->get(20 + i);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		real p = ((real(i) + engineControlTime() * 3e-6) % knotsCount) / knotsCount;
		r.opacity = smootherstep(clamp(p * 2, 0, 1));
	}

	// moving light bulbs
	for (uint32 i = 0; i < bulbsCount; i++)
	{
		Entity *e = ents->get(100 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
#if 1
		if (distance(t.position, vec3(0, 3, 0)) > 10 || t.position[1] < -1)
			t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
		vec3 &v = bulbsVelocities[i];
		v += bulbChange(t.position) * 0.1;
		v = normalize(v);
		t.position += v * 0.1;
#else
		rads angle = rads(degs(i * 360.0 / bulbsCount)) + degs(engineControlTime() * 5e-6);
		t.position = vec3(sin(angle) * 5, 1, cos(angle) * 5);
#endif
	}

	return false;
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
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("translucent");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(8, 6, 10);
			t.orientation = quat(degs(-30), degs(40), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.near = 0.1;
			c.far = 1000;
			c.effects = CameraEffectsFlags::CombinedPass;
			c.ambientLight = vec3(0.1);
			c.ambientDirectionalLight = vec3(0.2);
		}
		{ // sun
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			t.position = vec3(0, 5, 0);
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(2, 2, 1);
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(15);
		}
		{ // flash light
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-5, 4, 3);
			t.orientation = quat(degs(-40), degs(-110), degs());
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/bottle/other.obj?arrow");
			r.color = vec3(1);
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = degs(60);
			l.spotExponent = 40;
			l.attenuation = vec3(0, 0, 0.03);
			l.color = vec3(15);
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 1024;
			s.worldSize = vec3(3, 50, 0);
		}
		{ // floor
			Entity *e = ents->create(5);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Ground");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, -0.2, 0);
		}
		{ // flying knot
			Entity *e = ents->create(6);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Knot");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(5, 2, -5);
			t.scale = 3;
		}
		{ // suzanne
			Entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/suzanne.blend");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(2, 1, 0);
		}
		{ // cube
			Entity *e = ents->create(11);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Cube");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-3, 1, -2);
			t.orientation = randomDirectionQuat();
		}
		{ // sphere
			Entity *e = ents->create(12);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Sphere");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-0.5, 1, 3.5);
			t.orientation = randomDirectionQuat();
		}
		{ // plane
			Entity *e = ents->create(13);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Plane");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-0.5, 1, -1);
			t.orientation = quat(degs(), degs(80), degs());
		}
		{ // bottle
			Entity *e = ents->create(14);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/bottle/bottle.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-1.1, -0.2, 5.8);
			t.scale = 0.6;
		}
		// animated knots
		for (uint32 i = 0; i < knotsCount; i++)
		{
			Entity *e = ents->create(20 + i);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Knot");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			rads angle = degs(i * 360.0 / knotsCount);
			t.position = vec3(sin(angle) * 8, 0.5, cos(angle) * 8);
			t.orientation = quat(degs(), angle, degs());
			t.scale = 0.6;
		}
		// animated bulbs
		for (uint32 i = 0; i < bulbsCount; i++)
		{
			Entity *e = ents->create(100 + i);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/translucent/shapes.blend?Bulb");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.scale = 0.1;
			t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Point;
			r.color = l.color = colorHsvToRgb(randomChance3() * vec3(1, .5, .5) + vec3(0, .5, .5));
			l.attenuation = vec3(0.5, 0, 0.07);
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
		Holder<EngineProfiling> EngineProfiling = newEngineProfiling();

		engineAssets()->add(assetsName);
		engineStart();
		engineAssets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
