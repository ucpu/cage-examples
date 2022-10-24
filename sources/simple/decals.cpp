#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/collider.h>
#include <cage-core/geometry.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/decals/decals.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	// nothing for now
}

void collisionPoint(const Transform &start, const Holder<Collider> &c, Vec3 &p, Vec3 &n)
{
	if (!c)
		return;
	const Vec3 o = start.position;
	const auto ray = makeRay(o, o + start.orientation * Vec3(0, 0, -1));
	uint32 id = m;
	const Vec3 q = intersection(ray, +c, Transform(), id);
	if (valid(q) && (!valid(p) || distanceSquared(o, q) < distanceSquared(o, p)))
	{
		p = q;
		n = c->triangles()[id].normal();
	}
}

std::pair<Vec3, Vec3> raycast()
{
	const Transform &start = engineEntities()->get(1)->value<TransformComponent>();
	Vec3 p = Vec3::Nan();
	Vec3 n = Vec3::Nan();
	collisionPoint(start, engineAssets()->get<AssetSchemeIndexCollider, Collider>(HashString("cage-tests/decals/floor.obj;collider")), p, n);
	collisionPoint(start, engineAssets()->get<AssetSchemeIndexCollider, Collider>(HashString("cage-tests/decals/pedestal.obj;collider")), p, n);
	collisionPoint(start, engineAssets()->get<AssetSchemeIndexCollider, Collider>(HashString("cage-tests/decals/statue.obj;collider")), p, n);
	return { p, n };
}

void keyPress(InputKey in)
{
	if (in.key != 32)
		return;

	const auto p = raycast();
	if (!valid(p.first))
		return;
	CAGE_ASSERT(valid(p.second));

	EntityManager *ents = engineEntities();
	Entity *e = ents->createAnonymous();
	e->value<RenderComponent>().object = HashString("cage-tests/decals/decal.obj");
	TransformComponent &t = e->value<TransformComponent>();
	t.position = p.first;
	t.orientation = Quat(p.second, randomDirection3(), false);
	t.scale = randomRange(Real(0.3), Real(0.8));
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
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();
		InputListener<InputClassEnum::KeyPress, InputKey> keyPressListener;
		keyPressListener.attach(engineWindow()->events);
		keyPressListener.bind<&keyPress>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("decals");

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
			c.ambientIntensity = 0.1;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.2;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			t.position = Vec3(0, 5, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(15);
		}
		{ // floor
			Entity *e = ents->create(5);
			e->value<RenderComponent>().object = HashString("cage-tests/decals/floor.obj");
			e->value<TransformComponent>();
		}
		{ // pedestal
			Entity *e = ents->create(6);
			e->value<RenderComponent>().object = HashString("cage-tests/decals/pedestal.obj");
			e->value<TransformComponent>();
		}
		{ // statue
			Entity *e = ents->create(7);
			e->value<RenderComponent>().object = HashString("cage-tests/decals/statue.obj");
			e->value<TransformComponent>();
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

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
