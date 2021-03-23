#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/geometry.h>
#include <cage-core/collider.h>
#include <cage-core/camera.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/sound.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/screamers/screamers.pack");
constexpr const uint32 assetsMusic[] = { HashString("cage-tests/music/PurplePlanet/BigDayOut.mp3"), HashString("cage-tests/music/PurplePlanet/Civilisation.mp3"), HashString("cage-tests/music/PurplePlanet/FunkCity.mp3"), HashString("cage-tests/music/PurplePlanet/IntoBattle.mp3"), HashString("cage-tests/music/PurplePlanet/RetroGamer.mp3"), HashString("cage-tests/music/PurplePlanet/TrueFaith.mp3") };
constexpr const uint32 assetsScreams[] = { HashString("cage-tests/screamers/screams/1.mp3"), HashString("cage-tests/screamers/screams/2.wav"), HashString("cage-tests/screamers/screams/3.ogg"), HashString("cage-tests/screamers/screams/4.wav") };
constexpr const uint32 assetsExplosionsSounds[] = { HashString("cage-tests/screamers/explosion/1.flac"), HashString("cage-tests/screamers/explosion/2.wav"), HashString("cage-tests/screamers/explosion/3.wav") };
constexpr const uint32 assetsExplosionsSprites[] = { HashString("cage-tests/explosion/sprite.obj;explosion_1"), HashString("cage-tests/explosion/sprite.obj;explosion_2"), HashString("cage-tests/explosion/sprite.obj;explosion_3") };
constexpr const uint32 assetsSmokes[] = { HashString("cage-tests/explosion/sprite.obj;smoke_1"), HashString("cage-tests/explosion/sprite.obj;smoke_2") };
constexpr const uint32 assetsSparks[] = { HashString("cage-tests/explosion/sprite.obj;spark_1") };
const vec3 cameraCenter = vec3(0, 2, 0);

struct ScreamerComponent
{
	static EntityComponent *component;
	vec3 velocity;
	uint32 steaming = 0;
};

struct TtlComponent
{
	static EntityComponent *component;
	uint32 ttl = 0;
};

struct ParticleComponent
{
	static EntityComponent *component;
	vec3 velocity;
	real gravity;
	real drag;
	real smoking; // a chance to create a smoke particle
	real dimming = real::Nan();
};

EntityComponent *ScreamerComponent::component;
EntityComponent *TtlComponent::component;
EntityComponent *ParticleComponent::component;

EntityGroup *entsToDestroy;

uint32 arrayPick(PointerRange<const uint32> arr)
{
	return arr[randomRange(uintPtr(0), arr.size())];
}

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
	ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
	TtlComponent &ttlc = e->value<TtlComponent>(TtlComponent::component);
	ttlc.ttl = ttl;
	return e;
}

Entity *makeSmoke(const vec3 &position, real scale)
{
	Entity *e = makeParticle(position, scale, arrayPick(assetsSmokes), randomRange(20u, 30u));
	ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
	p.drag = 0.03;
	p.velocity = vec3(randomRange(-0.2, 0.2), randomRange(0.5, 1.0), randomRange(-0.2, 0.2)) / 20 * scale;
	return e;
}

void makeExplosion(const vec3 &position)
{
	{
		const uint32 explosions = randomRange(3u, 5u);
		for (uint32 i = 0; i < explosions; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance(), randomRange(0.5, 1.0), arrayPick(assetsExplosionsSprites), randomRange(30u, 60u));
			ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
			p.smoking = 0.1;
			p.dimming = 0.1;
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.color = vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = saturate(l.color);
			l.intensity = 8;
			l.attenuation = vec3(0, 1, 0);
		}
	}
	{
		const uint32 sparks = randomRange(3u, 7u);
		for (uint32 i = 0; i < sparks; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance() * 0.5, randomRange(0.1, 0.3), arrayPick(assetsSparks), 40);
			ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
			p.smoking = 0.4;
			p.gravity = 0.5 / 20;
			p.drag = 0.05;
			p.velocity = vec3(randomRange(-10.0, 10.0), randomRange(5.0, 10.0), randomRange(-10.0, 10.0)) / 20;
			p.dimming = 0.1;
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.color = vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = saturate(l.color);
			l.intensity = 5;
			l.attenuation = vec3(0, 1, 0);
		}
	}
	{
		Entity *e = engineEntities()->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, tr, e);
		tr.position = position;
		tr.orientation = randomDirectionQuat();
		CAGE_COMPONENT_ENGINE(Sound, snd, e);
		snd.name = arrayPick(assetsExplosionsSounds);
		snd.startTime = engineControlTime();
		TtlComponent &ttlc = e->value<TtlComponent>(TtlComponent::component);
		Holder<Sound> s = engineAssets()->get<AssetSchemeIndexSound, Sound>(snd.name);
		ttlc.ttl = numeric_cast<uint32>(s ? s->duration() * 20 / 1000000 + 10 : 5 * 20);
	}
}

void updateParticles()
{
	std::vector<vec4> newSmokes;
	newSmokes.reserve(ParticleComponent::component->entities().size() / 10);
	for (Entity *e : ParticleComponent::component->entities())
	{
		ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		p.velocity *= 1 - p.drag;
		p.velocity += vec3(0, -p.gravity, 0);
		t.position += p.velocity;
		t.orientation = quat(t.position - cameraCenter, vec3(0, 1, 0));
		if (p.dimming.valid())
		{
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.intensity *= 1 - p.dimming;
			if (l.intensity < 0.05)
				e->remove(LightComponent::component);
		}
		if (randomChance() < p.smoking)
			newSmokes.emplace_back(vec4(t.position + randomDirection3() * (randomChance() * t.scale * 0.5), (randomChance() + 0.5) * t.scale));
	}
	for (const vec4 &s : newSmokes)
		makeSmoke(vec3(s), s[3]);
}

void spawnScreamer()
{
	Entity *e = engineEntities()->createUnique();

	CAGE_COMPONENT_ENGINE(Render, render, e);
	render.object = HashString("cage-tests/screamers/suzanne.blend");

	CAGE_COMPONENT_ENGINE(Transform, tr, e);
	tr.position = randomDirection3() * vec3(1, 0, 1) * randomRange(500, 700) + vec3(0, 10, 0);
	tr.orientation = randomDirectionQuat();
	tr.scale = randomRange(0.4, 0.6);

	CAGE_COMPONENT_ENGINE(Sound, snd, e);
	snd.name = arrayPick(assetsScreams);
	snd.startTime = engineControlTime();

	CAGE_COMPONENT_ENGINE(Light, lig, e);
	lig.attenuation = vec3(0, 1, 0);
	render.color = lig.color = randomDirection3() * 0.5 + 0.5;
	lig.intensity = 5;

	ScreamerComponent &scr = e->value<ScreamerComponent>(ScreamerComponent::component);
	scr.velocity = randomDirection3();
}

line getCursorRay()
{
	const ivec2 cursor = engineWindow()->mousePosition();
	const ivec2 res = engineWindow()->resolution();
	const vec2 p = vec2(cursor) / vec2(res) * 2 - 1;
	const real px = p[0], py = -p[1];
	Entity *camera = engineEntities()->get(1);
	CAGE_COMPONENT_ENGINE(Transform, ts, camera);
	CAGE_COMPONENT_ENGINE(Camera, cs, camera);
	const mat4 view = inverse(mat4(ts.position, ts.orientation, vec3(ts.scale, ts.scale, ts.scale)));
	const mat4 proj = perspectiveProjection(cs.camera.perspectiveFov, real(res[0]) / real(res[1]), cs.near, cs.far);
	const mat4 inv = inverse(proj * view);
	const vec4 pn = inv * vec4(px, py, -1, 1);
	const vec4 pf = inv * vec4(px, py, 1, 1);
	const vec3 near = vec3(pn) / pn[3];
	const vec3 far = vec3(pf) / pf[3];
	const line ray = makeRay(near, far);
	return ray;
}

void updateScreamers()
{
	Holder<Collider> collider = engineAssets()->get<AssetSchemeIndexCollider, Collider>(HashString("cage-tests/screamers/suzanne.blend;collider"));
	if (!collider)
		return;

	static uint32 updateCounter = 1;
	static uint32 screamersCapacity = 3;
	if ((updateCounter++ % (30 * 20)) == 0 && screamersCapacity < 20)
	{
		screamersCapacity++;
		CAGE_LOG(SeverityEnum::Info, "screamers", "more screamers!");
	}

	if (ScreamerComponent::component->group()->count() < screamersCapacity && randomChance() / screamersCapacity < 0.01)
		spawnScreamer();

	const line cursorRay = getCursorRay();

	std::vector<vec3> newSmokes, newExplosions;
	for (Entity *e : ScreamerComponent::component->group()->entities())
	{
		CAGE_COMPONENT_ENGINE(Transform, tr, e);

		if (intersects(cursorRay, +collider, tr))
		{ // hit the screamer
			CAGE_LOG(SeverityEnum::Info, "screamers", "poof");
			e->add(entsToDestroy);
			newExplosions.push_back(tr.position);
			continue;
		}

		const real dist = distance(cameraCenter, tr.position);
		if (dist < 2)
		{ // hit the player
			CAGE_LOG(SeverityEnum::Info, "screamers", "hit");
			e->add(entsToDestroy);
			continue;
		}

		ScreamerComponent &scr = e->value<ScreamerComponent>(ScreamerComponent::component);
		const vec3 dir = normalize(cameraCenter - tr.position);
		scr.velocity += dir * 0.1;
		scr.velocity += tr.orientation * vec3(0.2, 1, 0) * clamp(dist * 0.01, 0, 1) * 0.3;
		scr.velocity += vec3(0, 1, 0) * sqr(max(2 - tr.position[1], 0));
		tr.position += scr.velocity;
		tr.orientation = quat(scr.velocity, tr.orientation * vec3(0, 1, 0)) * quat(degs(), degs(), degs(20));
		scr.velocity *= 0.93;

		if (scr.steaming++ > 2)
		{
			newSmokes.push_back(tr.position + tr.orientation * vec3(randomRange2(-0.5, 0.5), 1.5));
			scr.steaming = randomRange(0, 3);
		}
	}
	for (const vec3 &p : newSmokes)
		makeSmoke(p, randomRange(0.3, 0.7));
	for (const vec3 &p : newExplosions)
		makeExplosion(p);
}

void windowClose()
{
	engineStop();
}

void update()
{
	EntityManager *ents = engineEntities();

	updateScreamers();
	updateParticles();

	// ttl
	for (Entity *e : TtlComponent::component->group()->entities())
	{
		TtlComponent &ttl = e->value<TtlComponent>(TtlComponent::component);
		if (ttl.ttl-- == 0)
			e->add(entsToDestroy);
	}

	entsToDestroy->destroy();
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

		EventListener<void()> windowCloseListener;
		windowCloseListener.attach(engineWindow()->events.windowClose);
		windowCloseListener.bind<&windowClose>();

		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("screamers");

		// entities
		EntityManager *ents = engineEntities();
		ScreamerComponent::component = ents->defineComponent(ScreamerComponent(), true);
		TtlComponent::component = ents->defineComponent(TtlComponent(), true);
		ParticleComponent::component = ents->defineComponent(ParticleComponent(), true);
		entsToDestroy = ents->defineGroup();
		{ // camera
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = cameraCenter;
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.near = 0.03;
			c.far = 500;
			c.effects = CameraEffectsFlags::Default;
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.1;
			c.ambientDirectionalColor = vec3(1);
			c.ambientDirectionalIntensity = 0.2;
			CAGE_COMPONENT_ENGINE(Listener, l, e);
			l.rolloffFactor = 0.1;
		}
		{ // skybox
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/screamers/skybox.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 2, 0);
			t.scale = 200;
		}
		{ // sun
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(1);
			l.intensity = 0.5;
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 4096;
			s.worldSize = vec3(50);
		}
		{ // floor
			Entity *e = ents->create(4);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("scenes/common/ground.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0;
		cameraCtrl->turningSpeed *= 0.5;
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
