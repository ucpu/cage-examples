#include <cage-core/logger.h>
#include <cage-core/entities.h>
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

constexpr uint32 assetsName = HashString("cage-tests/camera-effects/effects.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	// todo
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
		engineWindow()->setMaximized();

		// entities
		EntityManager *ents = engineEntities();
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
			TransformComponent &t = e->value<TransformComponent>();
			// todo t
			e->value<VrOriginComponent>().virtualReality = +engineVirtualReality();
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
			e->value<ScreenSpaceEffectsComponent>().effects |= ScreenSpaceEffectsFlags::EyeAdaptation;
		}
		{ // left controller
			Entity *e = ents->create(3);
			e->value<TransformComponent>();
			e->value<VrControllerComponent>().controller = &engineVirtualReality()->leftController();
		}
		{ // right controller
			Entity *e = ents->create(4);
			e->value<TransformComponent>();
			e->value<VrControllerComponent>().controller = &engineVirtualReality()->rightController();
		}

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
