#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;
constexpr uint32 assetNames[] = {
	HashString("scenes/mcguire/crytek/sponza.pack"),
	HashString("scenes/mcguire/dragon/dragon.pack"),
	HashString("scenes/mcguire/rungholt/house.pack"),
	HashString("cage-tests/translucent/translucent.pack"),
};
constexpr uint32 bulbsCount = 30;

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

	// moving light bulbs
	for (uint32 i = 0; i < bulbsCount; i++)
	{
		Entity *e = ents->get(100 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		if (distance(t.position, vec3(0, 3, 0)) > 10 || t.position[1] < -1)
			t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
		vec3 &v = bulbsVelocities[i];
		v += bulbChange(t.position) * 0.02;
		v = normalize(v);
		t.position += v * 0.02;
	}

	// camera transformations
	for (uint32 i = 2; i <= 4; i++)
	{
		CAGE_COMPONENT_ENGINE(Transform, s, ents->get(1));
		CAGE_COMPONENT_ENGINE(Transform, t, ents->get(i));
		t = s;
	}

	return false;
}

void initCommon()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(8, 6, 10);
		t.orientation = quat(degs(-30), degs(40), degs());
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.near = 0.1;
		c.far = 1000;
		c.effects = CameraEffectsFlags::Default;
		c.sceneMask = 1 + 2 + 4;
		c.viewportOrigin = vec2(0, 0);
		c.viewportSize = vec2(0.5);
		c.cameraOrder = 1;
	}

	{ // floor
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/translucent/shapes.blend?Ground");
		r.sceneMask = 1 + 2 + 4;
	}

	// animated light bulbs
	for (uint32 i = 0; i < bulbsCount; i++)
	{
		Entity *e = ents->create(100 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 0.015;
		t.position = randomDirection3() * randomRange(1, 5) + vec3(0, 3, 0);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/translucent/shapes.blend?Bulb");
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Point;
		l.attenuation = vec3(0.2, 0, 0.25);
		r.color = l.color = colorHsvToRgb(randomChance3() * vec3(1, .5, .5) + vec3(0, .5, .5));
		r.sceneMask = l.sceneMask = 0 + (randomChance() < 0.5 ? 1 : 0) + (randomChance() < 0.5 ? 2 : 0) + (randomChance() < 0.5 ? 4 : 0);
	}
	for (uint32 j = 0; j < 3; j++)
	{
		NoiseFunctionCreateConfig cfg;
		cfg.seed = j * 17 + 42 + randomRange(100, 1000);
		cfg.frequency *= 0.5;
		bulbsNoise[j] = newNoiseFunction(cfg);
	}
}

void initSceneCrytek()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.near = 0.1;
		c.far = 1000;
		c.effects = CameraEffectsFlags::Default;
		c.sceneMask = 1;
		c.viewportOrigin = vec2(0.5, 0);
		c.viewportSize = vec2(0.5);
		c.cameraOrder = 2;
	}

	{ // sun
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-50), degs(2 * 120), degs());
		t.position = vec3(0, 5, 0);
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Directional;
		l.color = vec3(0, 1, 1);
		l.intensity = 2;
		l.sceneMask = 1;
		CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
		s.resolution = 2048;
		s.worldSize = vec3(15);
	}

	{
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 0.4;
		t.position = vec3(0, 0.2, 0);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("scenes/mcguire/crytek/sponza.object");
		r.sceneMask = 1;
	}
}

void initSceneDragon()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(3);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.near = 0.1;
		c.far = 1000;
		c.effects = CameraEffectsFlags::Default;
		c.sceneMask = 2;
		c.viewportOrigin = vec2(0, 0.5);
		c.viewportSize = vec2(0.5);
		c.cameraOrder = 3;
	}

	{ // sun
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-50), degs(3 * 120), degs());
		t.position = vec3(0, 5, 0);
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Directional;
		l.color = vec3(1, 0, 1);
		l.intensity = 2;
		l.sceneMask = 2;
		CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
		s.resolution = 2048;
		s.worldSize = vec3(15);
	}

	{
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 0.2;
		t.position = vec3(0, 2, 0);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("scenes/mcguire/dragon/dragon.object");
		r.sceneMask = 2;
	}
}

void initSceneRungholt()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(4);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.near = 0.1;
		c.far = 1000;
		c.effects = CameraEffectsFlags::Default;
		c.sceneMask = 4;
		c.viewportOrigin = vec2(0.5, 0.5);
		c.viewportSize = vec2(0.5);
		c.cameraOrder = 4;
	}

	{ // sun
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-50), degs(4 * 120), degs());
		t.position = vec3(0, 5, 0);
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Directional;
		l.color = vec3(1, 1, 0);
		l.intensity = 2;
		l.sceneMask = 4;
		CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
		s.resolution = 2048;
		s.worldSize = vec3(15);
	}

	{
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 0.4;
		t.position = vec3(0, -0.5, 0);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("scenes/mcguire/rungholt/house.object");
		r.sceneMask = 4;
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
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("scenesIsolation");

		initCommon();
		initSceneCrytek();
		initSceneDragon();
		initSceneRungholt();

		Holder<FpsCamera> cameraCtrl = newFpsCamera(engineEntities()->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

		for (uint32 i : assetNames)
			engineAssets()->add(i);
		engineStart();
		for (uint32 i : assetNames)
			engineAssets()->remove(i);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
