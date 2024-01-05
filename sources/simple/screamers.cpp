#include <vector>

#include <cage-core/assetManager.h>
#include <cage-core/camera.h>
#include <cage-core/collider.h>
#include <cage-core/entities.h>
#include <cage-core/geometry.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sound.h>
#include <cage-engine/window.h>
#include <cage-simple/cameraRay.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/screamers/screamers.pack");
constexpr const uint32 assetsScreams[] = { HashString("cage-tests/screamers/screams/1.mp3"), HashString("cage-tests/screamers/screams/2.wav"), HashString("cage-tests/screamers/screams/3.ogg"), HashString("cage-tests/screamers/screams/4.wav") };
constexpr const uint32 assetsExplosionsSounds[] = { HashString("cage-tests/screamers/explosion/1.flac"), HashString("cage-tests/screamers/explosion/2.wav"), HashString("cage-tests/screamers/explosion/3.wav") };
constexpr const uint32 assetsExplosionsSprites[] = { HashString("cage-tests/explosion/sprite.obj;explosion_1"), HashString("cage-tests/explosion/sprite.obj;explosion_2"), HashString("cage-tests/explosion/sprite.obj;explosion_3") };
constexpr const uint32 assetsSmokes[] = { HashString("cage-tests/explosion/sprite.obj;smoke_1"), HashString("cage-tests/explosion/sprite.obj;smoke_2") };
constexpr const uint32 assetsSparks[] = { HashString("cage-tests/explosion/sprite.obj;spark_1") };
const Vec3 cameraCenter = Vec3(0, 2, 0);

struct ScreamerComponent
{
	static EntityComponent *component;
	Vec3 velocity;
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
	Vec3 velocity;
	Real gravity;
	Real drag;
	Real smoking; // a chance to create a smoke particle
	Real dimming = Real::Nan();
};

struct DestroyingComponent
{
	static EntityComponent *component;
};

EntityComponent *ScreamerComponent::component;
EntityComponent *TtlComponent::component;
EntityComponent *ParticleComponent::component;
EntityComponent *DestroyingComponent::component;

uint32 arrayPick(PointerRange<const uint32> arr)
{
	return arr[randomRange(uintPtr(0), arr.size())];
}

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
	e->value<ParticleComponent>(ParticleComponent::component);
	TtlComponent &ttlc = e->value<TtlComponent>(TtlComponent::component);
	ttlc.ttl = ttl;
	return e;
}

Entity *makeSmoke(const Vec3 &position, Real scale)
{
	Entity *e = makeParticle(position, scale, arrayPick(assetsSmokes), randomRange(20u, 30u));
	ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
	p.drag = 0.03;
	p.velocity = Vec3(randomRange(-0.2, 0.2), randomRange(0.5, 1.0), randomRange(-0.2, 0.2)) / 20 * scale;
	return e;
}

void makeExplosion(const Vec3 &position)
{
	{
		const uint32 explosions = randomRange(3u, 5u);
		for (uint32 i = 0; i < explosions; i++)
		{
			Entity *e = makeParticle(position + randomDirection3() * randomChance(), randomRange(0.5, 1.0), arrayPick(assetsExplosionsSprites), randomRange(30u, 60u));
			ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
			p.smoking = 0.1;
			p.dimming = 0.1;
			LightComponent &l = e->value<LightComponent>();
			l.color = Vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = saturate(l.color);
			l.intensity = 8;
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
			p.velocity = Vec3(randomRange(-10.0, 10.0), randomRange(5.0, 10.0), randomRange(-10.0, 10.0)) / 20;
			p.dimming = 0.1;
			LightComponent &l = e->value<LightComponent>();
			l.color = Vec3(247, 221, 59) / 255;
			l.color += randomRange3(-0.05, 0.05);
			l.color = saturate(l.color);
			l.intensity = 5;
		}
	}
	{
		Entity *e = engineEntities()->createAnonymous();
		TransformComponent &tr = e->value<TransformComponent>();
		tr.position = position;
		tr.orientation = randomDirectionQuat();
		SoundComponent &snd = e->value<SoundComponent>();
		snd.name = arrayPick(assetsExplosionsSounds);
		snd.startTime = engineControlTime();
		TtlComponent &ttlc = e->value<TtlComponent>(TtlComponent::component);
		Holder<Sound> s = engineAssets()->get<AssetSchemeIndexSound, Sound>(snd.name);
		ttlc.ttl = numeric_cast<uint32>(s ? s->duration() * 20 / 1000000 + 10 : 5 * 20);
	}
}

void updateParticles()
{
	std::vector<Vec4> newSmokes;
	newSmokes.reserve(ParticleComponent::component->entities().size() / 10);
	for (Entity *e : ParticleComponent::component->entities())
	{
		ParticleComponent &p = e->value<ParticleComponent>(ParticleComponent::component);
		TransformComponent &t = e->value<TransformComponent>();
		p.velocity *= 1 - p.drag;
		p.velocity += Vec3(0, -p.gravity, 0);
		t.position += p.velocity;
		t.orientation = Quat(t.position - cameraCenter, Vec3(0, 1, 0));
		if (p.dimming.valid())
		{
			LightComponent &l = e->value<LightComponent>();
			l.intensity *= 1 - p.dimming;
			if (l.intensity < 0.05)
				e->remove<LightComponent>();
		}
		if (randomChance() < p.smoking)
			newSmokes.emplace_back(Vec4(t.position + randomDirection3() * (randomChance() * t.scale * 0.5), (randomChance() + 0.5) * t.scale));
	}
	for (const Vec4 &s : newSmokes)
		makeSmoke(Vec3(s), s[3]);
}

void spawnScreamer()
{
	Entity *e = engineEntities()->createUnique();

	RenderComponent &render = e->value<RenderComponent>();
	render.object = HashString("cage-tests/screamers/suzanne.blend");

	TransformComponent &tr = e->value<TransformComponent>();
	tr.position = randomDirection3() * Vec3(1, 0, 1) * randomRange(500, 700) + Vec3(0, 10, 0);
	tr.orientation = randomDirectionQuat();
	tr.scale = randomRange(0.4, 0.6);

	SoundComponent &snd = e->value<SoundComponent>();
	snd.name = arrayPick(assetsScreams);
	snd.startTime = engineControlTime();

	LightComponent &lig = e->value<LightComponent>();
	render.color = lig.color = randomDirection3() * 0.5 + 0.5;
	lig.intensity = 5;

	ScreamerComponent &scr = e->value<ScreamerComponent>(ScreamerComponent::component);
	scr.velocity = randomDirection3();
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

	if (ScreamerComponent::component->count() < screamersCapacity && randomChance() / screamersCapacity < 0.01)
		spawnScreamer();

	const Line cursorRay = cameraMouseRay(engineEntities()->get(1));

	std::vector<Vec3> newSmokes, newExplosions;
	for (Entity *e : ScreamerComponent::component->entities())
	{
		TransformComponent &tr = e->value<TransformComponent>();

		if (intersects(cursorRay, +collider, tr))
		{ // hit the screamer
			CAGE_LOG(SeverityEnum::Info, "screamers", "poof");
			e->value<DestroyingComponent>();
			newExplosions.push_back(tr.position);
			continue;
		}

		const Real dist = distance(cameraCenter, tr.position);
		if (dist < 2)
		{ // hit the player
			CAGE_LOG(SeverityEnum::Info, "screamers", "hit");
			e->value<DestroyingComponent>();
			continue;
		}

		ScreamerComponent &scr = e->value<ScreamerComponent>(ScreamerComponent::component);
		const Vec3 dir = normalize(cameraCenter - tr.position);
		scr.velocity += dir * 0.1;
		scr.velocity += tr.orientation * Vec3(0.2, 1, 0) * clamp(dist * 0.01, 0, 1) * 0.3;
		scr.velocity += Vec3(0, 1, 0) * sqr(max(2 - tr.position[1], 0));
		tr.position += scr.velocity;
		tr.orientation = Quat(scr.velocity, tr.orientation * Vec3(0, 1, 0)) * Quat(Degs(), Degs(), Degs(20));
		scr.velocity *= 0.93;

		if (scr.steaming++ > 2)
		{
			newSmokes.push_back(tr.position + tr.orientation * Vec3(randomRange2(-0.5, 0.5), 1.5));
			scr.steaming = randomRange(0, 3);
		}
	}
	for (const Vec3 &p : newSmokes)
		makeSmoke(p, randomRange(0.3, 0.7));
	for (const Vec3 &p : newExplosions)
		makeExplosion(p);
}

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	updateScreamers();
	updateParticles();

	// ttl
	for (Entity *e : TtlComponent::component->entities())
	{
		TtlComponent &ttl = e->value<TtlComponent>(TtlComponent::component);
		if (ttl.ttl-- == 0)
			e->value<DestroyingComponent>();
	}

	DestroyingComponent::component->destroy();
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
		const auto closeListener = engineWindow()->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("screamers");

		// entities
		EntityManager *ents = engineEntities();
		ScreamerComponent::component = ents->defineComponent(ScreamerComponent());
		TtlComponent::component = ents->defineComponent(TtlComponent());
		ParticleComponent::component = ents->defineComponent(ParticleComponent());
		DestroyingComponent::component = ents->defineComponent(DestroyingComponent());
		{ // camera
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = cameraCenter;
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.03;
			c.far = 500;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
			e->value<ListenerComponent>().rolloffFactor = 0.1;
		}
		{ // skybox
			Entity *e = ents->create(2);
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("cage-tests/screamers/skybox.obj");
		}
		{ // sun
			Entity *e = ents->create(3);
			e->value<TransformComponent>().orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 0.5;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 4096;
			s.worldSize = Vec3(50);
		}
		{ // floor
			Entity *e = ents->create(4);
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0;
		cameraCtrl->turningSpeed *= 0.5;
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
