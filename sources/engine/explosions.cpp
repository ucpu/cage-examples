#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/macros.h>
#include <cage-core/camera.h>
#include <cage-core/geometry.h>
#include <cage-core/collider.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/explosion/explosion.pack");
constexpr const uint32 assetsExplosions[] = { HashString("cage-tests/explosion/sprite.obj;explosion_1"), HashString("cage-tests/explosion/sprite.obj;explosion_2"), HashString("cage-tests/explosion/sprite.obj;explosion_3") };
constexpr const uint32 assetsSmokes[] = { HashString("cage-tests/explosion/sprite.obj;smoke_1"), HashString("cage-tests/explosion/sprite.obj;smoke_2") };
constexpr const uint32 assetsSparks[] = { HashString("cage-tests/explosion/sprite.obj;spark_1") };
EntityGroup *entitiesToDestroy;

uint32 pickExplosion()
{
	return assetsExplosions[randomRange(uintPtr(0), sizeof(assetsExplosions) / sizeof(assetsExplosions[0]))];
}

uint32 pickSmoke()
{
	return assetsSmokes[randomRange(uintPtr(0), sizeof(assetsSmokes) / sizeof(assetsSmokes[0]))];
}

uint32 pickSparks()
{
	return assetsSparks[randomRange(uintPtr(0), sizeof(assetsSparks) / sizeof(assetsSparks[0]))];
}

struct Particle
{
	static EntityComponent *component;

	vec3 velocity;
	real gravity;
	real drag;
	real smoking; // a chance to create a smoke particle
	real dimming = real::Nan();
	uint32 ttl = 0;
};

EntityComponent *Particle::component;

Entity *makeParticle(const vec3 &position, real scale, uint32 object, uint32 ttl)
{
	Entity *e = engineEntities()->createAnonymous();
	CAGE_COMPONENT_ENGINE(Transform, t, e);
	t.position = position;
	t.scale = scale;
	CAGE_COMPONENT_ENGINE(Render, r, e);
	r.object = object;
	CAGE_COMPONENT_ENGINE(TextureAnimation, ta, e);
	ta.startTime = engineControlTime();
	ta.speed = 18.0 / ttl;
	Particle &p = e->value<Particle>(Particle::component);
	p.ttl = ttl;
	return e;
}

Entity *makeSmoke(const vec3 &position, real scale)
{
	Entity *e = makeParticle(position, scale, pickSmoke(), randomRange(20u, 30u));
	Particle &p = e->value<Particle>(Particle::component);
	p.drag = 0.03;
	p.velocity = vec3(randomRange(-0.2, 0.2), randomRange(0.5, 1.0), randomRange(-0.2, 0.2)) / 20 * scale;
	return e;
}

void makeExplosion(const vec3 &position)
{
	{
		uint32 explosions = randomRange(3u, 5u);
		for (uint32 i = 0; i < explosions; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance(), randomRange(0.5, 1.0), pickExplosion(), randomRange(30u, 60u));
			Particle &p = e->value<Particle>(Particle::component);
			p.smoking = 0.1;
			p.dimming = 0.2;
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.color = vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = clamp(l.color, 0, 1);
			l.intensity = randomRange(2.0, 4.0);
			l.attenuation = vec3(0, 0.5, 0.5);
		}
	}
	{
		uint32 sparks = randomRange(3u, 7u);
		for (uint32 i = 0; i < sparks; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance() * 0.5, randomRange(0.1, 0.3), pickSparks(), 40);
			Particle &p = e->value<Particle>(Particle::component);
			p.smoking = 0.4;
			p.gravity = 0.5 / 20;
			p.drag = 0.05;
			p.velocity = vec3(randomRange(-10.0, 10.0), randomRange(5.0, 10.0), randomRange(-10.0, 10.0)) / 20;
		}
	}
}

void windowClose()
{
	engineStop();
}

void mousePress(MouseButtonsFlags buttons, ModifiersFlags, const ivec2 &iPoint)
{
	if (none(buttons & MouseButtonsFlags::Left))
		return;

	vec3 position;
	{
		ivec2 res = engineWindow()->resolution();
		vec2 p = vec2(iPoint[0], iPoint[1]);
		p /= vec2(res[0], res[1]);
		p = p * 2 - 1;
		real px = p[0], py = -p[1];
		Entity *camera = engineEntities()->get(1);
		CAGE_COMPONENT_ENGINE(Transform, ts, camera);
		CAGE_COMPONENT_ENGINE(Camera, cs, camera);
		mat4 view = inverse(mat4(ts.position, ts.orientation, vec3(ts.scale, ts.scale, ts.scale)));
		mat4 proj = perspectiveProjection(cs.camera.perspectiveFov, real(res[0]) / real(res[1]), cs.near, cs.far);
		mat4 inv = inverse(proj * view);
		vec4 pn = inv * vec4(px, py, -1, 1);
		vec4 pf = inv * vec4(px, py, 1, 1);
		vec3 near = vec3(pn) / pn[3];
		vec3 far = vec3(pf) / pf[3];
		Line ray = makeRay(near, far);
		Plane floor = Plane(vec3(), vec3(0, 1, 0));
		position = intersection(ray, floor);
		if (!position.valid())
			return; // click outside floor
	}

	makeExplosion(position);
}

void reflectParticleVelocity(Particle &p, TransformComponent &t, Collider *collider)
{
	if (lengthSquared(p.velocity) < 1e-3)
		return;
	uint32 ti = m;
	if (intersection(makeSegment(t.position, t.position + p.velocity), collider, transform(), ti).valid())
	{
		const vec3 n = collider->triangles()[ti].normal();
		real d = dot(n, p.velocity);
		if (d < 0)
			p.velocity = p.velocity - 2 * d * n;
	}
}

void update()
{
	Holder<Collider> collider = engineAssets()->get<AssetSchemeIndexCollider, Collider>(HashString("cage-tests/explosion/floor.obj;collider"));
	if (!collider)
		return;
	
	EntityManager *ents = engineEntities();

	vec3 cameraCenter;
	{
		Entity *camera = engineEntities()->get(1);
		CAGE_COMPONENT_ENGINE(Transform, ts, camera);
		cameraCenter = ts.position;
	}

	std::vector<vec4> newSmokes;
	newSmokes.reserve(Particle::component->entities().size() / 10);
	for (Entity *e : Particle::component->entities())
	{
		Particle &p = e->value<Particle>(Particle::component);
		if (p.ttl-- == 0)
		{
			entitiesToDestroy->add(e);
			continue;
		}
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		p.velocity *= 1 - p.drag;
		p.velocity += vec3(0, -p.gravity, 0);
		reflectParticleVelocity(p, t, collider.get());
		t.position += p.velocity;
		t.orientation = quat(t.position - cameraCenter, vec3(0, 1, 0));
		if (p.dimming.valid())
		{
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.intensity *= 1 - p.dimming;
		}
		if (randomChance() < p.smoking)
			newSmokes.emplace_back(vec4(t.position + randomDirection3() * (randomChance() * t.scale * 0.5), (randomChance() + 0.5) * t.scale));
	}
	for (const vec4 &s : newSmokes)
		makeSmoke(vec3(s), s[3]);

	entitiesToDestroy->destroy();
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
		EventListener<void(MouseButtonsFlags, ModifiersFlags, const ivec2 &)> mouseListener;
		mouseListener.attach(engineWindow()->events.mousePress);
		mouseListener.bind<&mousePress>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("explosions");

		// entities
		EntityManager *ents = engineEntities();
		entitiesToDestroy = ents->defineGroup();
		Particle::component = ents->defineComponent<Particle>(Particle(), EntityComponentCreateConfig(true));
		{ // camera
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 3, 10);
			t.orientation = quat(degs(-20), degs(), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.near = 0.1;
			c.far = 1000;
			c.effects = CameraEffectsFlags::Default;
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.1;
			c.ambientDirectionalColor = vec3(1);
			c.ambientDirectionalIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			t.position = vec3(0, 5, 0);
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.intensity = 2;
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(15);
		}
		{ // floor
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/explosion/floor.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position[1] = -0.5;
		}
		// explosions
		for (int i = -2; i < 3; i++)
			makeExplosion(vec3(i * 2, 0, 0));

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Right;
		cameraCtrl->movementSpeed = 0.3;
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

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
