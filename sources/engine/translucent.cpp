
#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assets.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/noise.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/translucent/translucent.pack");
static const uint32 knotsCount = 40;
static const uint32 bulbsCount = 5;

holder<noiseClass> bulbsNoise[3];
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
	entityManagerClass *ents = entities();

	{ // flying knot
		entityClass *e = ents->get(6);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.orientation = quat(degs(), degs(0.5), degs()) * t.orientation;
		t.position[1] += sin(degs(currentControlTime() * 1e-5)) * 0.05;
	}

	// rotate objects (except the plane)
	for (uint32 i = 10; i < 13; i++)
	{
		entityClass *e = ents->get(i);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.orientation = quat(degs(), degs(i * 2 * 0.05), degs()) * t.orientation;
	}

	// animated knot opacities
	for (uint32 i = 0; i < knotsCount; i++)
	{
		entityClass *e = ents->get(20 + i);
		ENGINE_GET_COMPONENT(render, r, e);
		// todo r.opacity = ...
	}

	// moving light bulbs
	for (uint32 i = 0; i < bulbsCount; i++)
	{
		entityClass *e = ents->get(100 + i);
		ENGINE_GET_COMPONENT(transform, t, e);
#if 1
		if (t.position.distance(vec3(0, 3, 0)) > 10)
			t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
		vec3 &v = bulbsVelocities[i];
		v += bulbChange(t.position) * 0.1;
		v = v.normalize();
		t.position += v * 0.1;
#else
		rads angle = rads(degs(i * 360.0 / bulbsCount)) + degs(currentControlTime() * 5e-6);
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
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("translucent");

		// entities
		entityManagerClass *ents = entities();
		{ // camera
			entityClass *e = ents->create(1);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(6, 6, 7);
			t.orientation = quat(degs(-30), degs(40), degs());
			ENGINE_GET_COMPONENT(camera, c, e);
			c.near = 0.1;
			c.far = 300;
			c.effects = cameraEffectsFlags::CombinedPass;
			c.ambientLight = vec3(0.02);
		}
		{ // sun
			entityClass *e = ents->create(2);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			t.position = vec3(0, 5, 0);
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(2);
			//ENGINE_GET_COMPONENT(shadowmap, s, e);
			//s.resolution = 2048;
			//s.worldSize = vec3(15);
		}
		{ // floor
			entityClass *e = ents->create(5);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Ground");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, -0.2, 0);
		}
		{ // flying knot
			entityClass *e = ents->create(6);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Knot");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(5, 2, -5);
			t.scale = 3;
		}
		{ // suzanne
			entityClass *e = ents->create(10);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/suzanne.blend");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(2, 1, 0);
		}
		{ // cube
			entityClass *e = ents->create(11);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Cube");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(-2, 1, 0);
			t.orientation = randomDirectionQuat();
		}
		{ // sphere
			entityClass *e = ents->create(12);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Sphere");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 1, 2);
			t.orientation = randomDirectionQuat();
		}
		{ // plane
			entityClass *e = ents->create(13);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Plane");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 1, -2);
		}
		// animated knots
		for (uint32 i = 0; i < knotsCount; i++)
		{
			entityClass *e = ents->create(20 + i);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Knot");
			ENGINE_GET_COMPONENT(transform, t, e);
			rads angle = degs(i * 360.0 / knotsCount);
			t.position = vec3(sin(angle) * 8, 0.5, cos(angle) * 8);
			t.orientation = quat(degs(), angle, degs());
			t.scale = 0.6;
		}
		// animated bulbs
		for (uint32 i = 0; i < bulbsCount; i++)
		{
			entityClass *e = ents->create(100 + i);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/translucent/shapes.blend?Bulb");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.scale = 0.1;
			t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Point;
			r.color = l.color = convertHsvToRgb(randomChance3() * vec3(1, .5, .5) + vec3(0, .5, .5));
			l.attenuation = vec3(0, 0, 0.1);
		}
		for (uint32 j = 0; j < 3; j++)
		{
			noiseCreateConfig cfg;
			cfg.seed = j * 17 + 42 + randomRange(100, 1000);
			cfg.frequency *= 0.5;
			bulbsNoise[j] = newNoise(cfg);
		}

		holder<cameraControllerClass> cameraController = newCameraController(ents->get(1));
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.3;
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

		assets()->add(assetsName);
		engineStart();
		assets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}