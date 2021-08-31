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
#include <cage-engine/engineStatistics.h>
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

	Vec3 velocity;
	Real gravity;
	Real drag;
	Real smoking; // a chance to create a smoke particle
	Real dimming = Real::Nan();
	uint32 ttl = 0;
};

EntityComponent *Particle::component;

Entity *makeParticle(const Vec3 &position, Real scale, uint32 object, uint32 ttl)
{
	Entity *e = engineEntities()->createAnonymous();
	TransformComponent &t = e->value<TransformComponent>();
	t.position = position;
	t.scale = scale;
	RenderComponent &r = e->value<RenderComponent>();
	r.object = object;
	TextureAnimationComponent &ta = e->value<TextureAnimationComponent>();
	ta.startTime = engineControlTime();
	ta.speed = 18.0 / ttl;
	Particle &p = e->value<Particle>(Particle::component);
	p.ttl = ttl;
	return e;
}

Entity *makeSmoke(const Vec3 &position, Real scale)
{
	Entity *e = makeParticle(position, scale, pickSmoke(), randomRange(20u, 30u));
	Particle &p = e->value<Particle>(Particle::component);
	p.drag = 0.03;
	p.velocity = Vec3(randomRange(-0.2, 0.2), randomRange(0.5, 1.0), randomRange(-0.2, 0.2)) / 20 * scale;
	return e;
}

void makeExplosion(const Vec3 &position)
{
	{
		uint32 explosions = randomRange(3u, 5u);
		for (uint32 i = 0; i < explosions; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance(), randomRange(0.5, 1.0), pickExplosion(), randomRange(30u, 60u));
			Particle &p = e->value<Particle>(Particle::component);
			p.smoking = 0.1;
			p.dimming = 0.2;
			LightComponent &l = e->value<LightComponent>();
			l.color = Vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = clamp(l.color, 0, 1);
			l.intensity = randomRange(2.0, 4.0);
			l.attenuation = Vec3(0, 0.5, 0.5);
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
			p.velocity = Vec3(randomRange(-10.0, 10.0), randomRange(5.0, 10.0), randomRange(-10.0, 10.0)) / 20;
		}
	}
}

void windowClose()
{
	engineStop();
}

void mousePress(MouseButtonsFlags buttons, ModifiersFlags, const Vec2i &iPoint)
{
	if (none(buttons & MouseButtonsFlags::Left))
		return;

	Vec3 position;
	{
		Vec2i res = engineWindow()->resolution();
		Vec2 p = Vec2(iPoint[0], iPoint[1]);
		p /= Vec2(res[0], res[1]);
		p = p * 2 - 1;
		Real px = p[0], py = -p[1];
		Entity *camera = engineEntities()->get(1);
		TransformComponent &ts = camera->value<TransformComponent>();
		CameraComponent &cs = camera->value<CameraComponent>();
		Mat4 view = inverse(Mat4(ts.position, ts.orientation, Vec3(ts.scale, ts.scale, ts.scale)));
		Mat4 proj = perspectiveProjection(cs.camera.perspectiveFov, Real(res[0]) / Real(res[1]), cs.near, cs.far);
		Mat4 inv = inverse(proj * view);
		Vec4 pn = inv * Vec4(px, py, -1, 1);
		Vec4 pf = inv * Vec4(px, py, 1, 1);
		Vec3 near = Vec3(pn) / pn[3];
		Vec3 far = Vec3(pf) / pf[3];
		Line ray = makeRay(near, far);
		Plane floor = Plane(Vec3(), Vec3(0, 1, 0));
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
	if (intersection(makeSegment(t.position, t.position + p.velocity), collider, Transform(), ti).valid())
	{
		const Vec3 n = collider->triangles()[ti].normal();
		Real d = dot(n, p.velocity);
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

	Vec3 cameraCenter;
	{
		Entity *camera = engineEntities()->get(1);
		TransformComponent &ts = camera->value<TransformComponent>();
		cameraCenter = ts.position;
	}

	std::vector<Vec4> newSmokes;
	newSmokes.reserve(Particle::component->entities().size() / 10);
	for (Entity *e : Particle::component->entities())
	{
		Particle &p = e->value<Particle>(Particle::component);
		if (p.ttl-- == 0)
		{
			entitiesToDestroy->add(e);
			continue;
		}
		TransformComponent &t = e->value<TransformComponent>();
		p.velocity *= 1 - p.drag;
		p.velocity += Vec3(0, -p.gravity, 0);
		reflectParticleVelocity(p, t, collider.get());
		t.position += p.velocity;
		t.orientation = Quat(t.position - cameraCenter, Vec3(0, 1, 0));
		if (p.dimming.valid())
		{
			LightComponent &l = e->value<LightComponent>();
			l.intensity *= 1 - p.dimming;
		}
		if (randomChance() < p.smoking)
			newSmokes.emplace_back(Vec4(t.position + randomDirection3() * (randomChance() * t.scale * 0.5), (randomChance() + 0.5) * t.scale));
	}
	for (const Vec4 &s : newSmokes)
		makeSmoke(Vec3(s), s[3]);

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
		EventListener<void(MouseButtonsFlags, ModifiersFlags, const Vec2i &)> mouseListener;
		mouseListener.attach(engineWindow()->events.mousePress);
		mouseListener.bind<&mousePress>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("explosions");

		// entities
		EntityManager *ents = engineEntities();
		entitiesToDestroy = ents->defineGroup();
		Particle::component = ents->defineComponent<Particle>(Particle());
		{ // camera
			Entity *e = ents->create(1);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 3, 10);
			t.orientation = Quat(Degs(-20), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.effects = CameraEffectsFlags::Default;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.1;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			t.position = Vec3(0, 5, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(15);
		}
		{ // floor
			Entity *e = ents->create(3);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/explosion/floor.obj");
			TransformComponent &t = e->value<TransformComponent>();
			t.position[1] = -0.5;
		}
		// explosions
		for (int i = -2; i < 3; i++)
			makeExplosion(Vec3(i * 2, 0, 0));

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Right;
		cameraCtrl->movementSpeed = 0.3;
		Holder<EngineStatistics> statistics = newEngineStatistics();

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
