#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneVirtualReality.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-engine/virtualReality.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <cage-simple/engine.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

constexpr uint32 assetsName1 = HashString("cage-tests/camera-effects/effects.pack");
constexpr uint32 assetsName2 = HashString("cage-tests/vr/vr.pack");

void windowClose(InputWindow)
{
	engineStop();
}

struct ShotComponent
{
	uint32 ttl = 600;
};

void shoot(const Transform &where)
{
	auto ents = engineEntities();
	Entity *e = ents->createAnonymous();
	e->value<TransformComponent>() = where;
	e->value<TransformComponent>().scale = 0.04;
	e->value<ShotComponent>();
	e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
}

void update()
{
	auto ents = engineEntities();
	ents->get(5)->value<TransformComponent>() = ents->get(3)->value<VrControllerComponent>().aim;
	ents->get(6)->value<TransformComponent>() = ents->get(4)->value<VrControllerComponent>().aim;

	const auto leftAxes = engineVirtualReality()->leftController().axes();
	const auto rightAxes = engineVirtualReality()->rightController().axes();
	const Real turning = leftAxes[0] * 1;
	const Real moving = rightAxes[1] * 0.03;
	auto &t = ents->get(1)->value<TransformComponent>();
	t.orientation = Quat(Degs(), Degs(-turning), Degs()) * t.orientation;
	t.position += t.orientation * Vec3(0, 0, -moving);

	if (engineVirtualReality()->leftController().buttons()[1])
		virtualRealitySceneRecenter(ents, 1.3); // 1.3 = sitting height; 1.7 = standing height

	if (engineVirtualReality()->rightController().buttons()[0])
		shoot(ents->get(1)->value<TransformComponent>() * Transform(Vec3(0, 0.7, 0)));
	if (engineVirtualReality()->leftController().buttons()[8] || engineVirtualReality()->leftController().axes()[4] > 0.5)
		shoot(ents->get(5)->value<TransformComponent>());
	if (engineVirtualReality()->rightController().buttons()[8] || engineVirtualReality()->rightController().axes()[4] > 0.5)
		shoot(ents->get(6)->value<TransformComponent>());

	entitiesVisitor([&](Entity *e, TransformComponent &t, ShotComponent &s) {
		if (s.ttl-- == 0)
			e->destroy();
		else
			t.position += t.orientation * Vec3(0, 0, -0.12);
	}, ents, true);
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		{
			VirtualRealityCreateConfig v;
			EngineCreateConfig e;
			e.virtualReality = &v;
			engineInitialize(e);
		}

		// events
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		// window
		engineWindow()->title("virtual reality");
		engineWindow()->windowedSize(Vec2i(800, 600));
		engineWindow()->setWindowed();

		// entities
		EntityManager *ents = engineEntities();
		ents->defineComponent(ShotComponent());
		{ // skybox
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/skybox.obj");
			e->value<TextureAnimationComponent>();
		}
		{ // sun
			Entity *e = ents->createAnonymous();
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 0);
			t.orientation = Quat(Degs(-75), Degs(-120), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(30);
		}
		{ // floor
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
			e->value<TransformComponent>().position = Vec3(0, -1.264425, 0);
		}
		{ // sponza
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/mcguire/crytek/sponza.object");
			e->value<TransformComponent>();
		}
		{ // vr origin
			Entity *e = ents->create(1);
			e->value<TransformComponent>();
			e->value<VrOriginComponent>().virtualReality = +engineVirtualReality();
			e->value<RenderComponent>().object = HashString("cage-tests/vr/wheelchair.glb");
		}
		{ // vr camera
			Entity *e = ents->create(2);
			e->value<TransformComponent>();
			VrCameraComponent &c = e->value<VrCameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.02;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.04;
			c.virtualReality = +engineVirtualReality();
			e->value<ScreenSpaceEffectsComponent>().effects = (ScreenSpaceEffectsFlags::Default | ScreenSpaceEffectsFlags::EyeAdaptation) & ~ScreenSpaceEffectsFlags::AmbientOcclusion;
		}
		{ // left controller grip
			Entity *e = ents->create(3);
			e->value<TransformComponent>();
			e->value<VrControllerComponent>().controller = &engineVirtualReality()->leftController();
			e->value<RenderComponent>().object = HashString("cage-tests/vr/grip.obj");
		}
		{ // right controller grip
			Entity *e = ents->create(4);
			e->value<TransformComponent>();
			e->value<VrControllerComponent>().controller = &engineVirtualReality()->rightController();
			e->value<RenderComponent>().object = HashString("cage-tests/vr/grip.obj");
		}
		{ // left controller aim
			Entity *e = ents->create(5);
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("cage-tests/vr/aim.obj");
		}
		{ // right controller aim
			Entity *e = ents->create(6);
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("cage-tests/vr/aim.obj");
		}

		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName1);
		engineAssets()->add(assetsName2);
		engineStart();
		engineAssets()->remove(assetsName1);
		engineAssets()->remove(assetsName2);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}