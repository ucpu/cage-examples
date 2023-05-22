#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/string.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/skeletons/skeletons.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void label(const String &name, const Vec3 &position)
{
	Entity *e = engineEntities()->createAnonymous();
	TextComponent &txt = e->value<TextComponent>();
	txt.value = name;
	TransformComponent &t = e->value<TransformComponent>();
	t.position = position;
	t.scale = 0.2;
	t.orientation = Quat(Degs(), Degs(180), Degs());
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
		const auto closeListener = engineWindow()->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("skeletal animation");

		// entities
		EntityManager *ents = engineEntities();
		{ // lemurs
			constexpr const char *animations[] = { "cage-tests/skeletons/lemur/lemur.x?AttackMelee", "cage-tests/skeletons/lemur/lemur.x?idle", "cage-tests/skeletons/lemur/lemur.x?run" };
			uint32 i = 0;
			for (const char *animation : animations)
			{
				Entity *e = ents->create(1 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/lemur/lemur.x");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString(animation);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 3);
				label(remove(String(animation), 0, 35), t.position + Vec3(0, 2, 0));
				i++;
			}
			{ // no animation
				Entity *e = ents->create(1 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/lemur/lemur.x");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 3);
				label("no animation", t.position + Vec3(0, 2, 0));
				i++;
			}
			{ // scaled
				Entity *e = ents->create(1 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/lemur/lemur.x");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString("cage-tests/skeletons/lemur/lemur.x?idle");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 3);
				t.scale = 1.5;
				label("scaled", t.position + Vec3(0, 3, 0));
				i++;
			}
#if 1
			// performance
			for (sint32 i = 0; i < 32 * 32; i++)
			{
				Entity *e = ents->createAnonymous();
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/lemur/lemur.x");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString(animations[1]);
				s.speed = randomRange(0.1, 10.0);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3((i % 32) * 2 - 32, 0, (i / 32) * 2 - 70);
			}
#endif
		}
		{ // cylinders
			constexpr const char *animations[] = { "cage-tests/skeletons/cylinder/cylinder.x?bend", "cage-tests/skeletons/cylinder/cylinder.x?curve" };
			uint32 i = 0;
			for (const char *animation : animations)
			{
				Entity *e = ents->create(10 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/cylinder/cylinder.x");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString(animation);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 0);
				label(remove(String(animation), 0, 41), t.position + Vec3(0, 3.3, 0));
				i++;
			}
			{ // no animation set
				Entity *e = ents->create(10 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/cylinder/cylinder.x");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 0);
				label("no anim set", t.position + Vec3(0, 3.3, 0));
				i++;
			}
			{ // non-existent animation
				Entity *e = ents->create(10 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/cylinder/cylinder.x");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString("cage-tests/skeletons/cylinder/cylinder.x?non-existent");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 0);
				label("invalid anim", t.position + Vec3(0, 3.3, 0));
				i++;
			}
			{ // non-existent object
				Entity *e = ents->create(10 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/cylinder/non-existent.x");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, 0);
				label("invalid object", t.position + Vec3(0, 3.3, 0));
				i++;
			}
		}
		{ // monks
			constexpr const char *animations[] = { "cage-tests/skeletons/monk/monk.x?Attack1", "cage-tests/skeletons/monk/monk.x?Dance", "cage-tests/skeletons/monk/monk.x?Die" };
			uint32 i = 0;
			for (const char *animation : animations)
			{
				Entity *e = ents->create(20 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/monk/monk.object");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString(animation);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, -3);
				label(remove(String(animation), 0, 33), t.position + Vec3(0, 2, 0));
				i++;
			}
			{ // no animation
				Entity *e = ents->create(20 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/monk/monk.object");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, -3);
				label("no anim", t.position + Vec3(0, 2, 0));
				i++;
			}
			{ // rotated
				Entity *e = ents->create(20 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString("cage-tests/skeletons/monk/monk.object");
				SkeletalAnimationComponent &s = e->value<SkeletalAnimationComponent>();
				s.name = HashString("cage-tests/skeletons/monk/monk.x?Dance");
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 0, -3);
				t.orientation = Quat(Degs(), Degs(randomRange(45, 270)), Degs());
				label("rotated", t.position + Vec3(0, 2, 0));
				i++;
			}
		}
		{ // gimbals/spawners
			uint32 i = 0;
			constexpr const char *objects[] = { "cage-tests/skeletons/gimbal/spawner_fbx.object", "cage-tests/skeletons/gimbal/spawner_glb.object", "cage-tests/skeletons/gimbal/spawner_x.object" };
			for (const char *object : objects)
			{
				Entity *e = ents->create(30 + i);
				RenderComponent &r = e->value<RenderComponent>();
				r.object = HashString(object);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 + 1.5f, 1, 0);
				t.scale = 0.6;
				label(remove(String(object), 0, 28), t.position + Vec3(0, 1, 0));
				i++;
			}
		}
		{ // floor
			Entity *e = ents->create(100);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/skeletons/floor/floor.obj");
			TransformComponent &t = e->value<TransformComponent>();
			(void)t;
		}
		{ // sun
			Entity *e = ents->create(101);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(12, 12, 10);
		}
		{ // camera
			Entity *e = ents->create(102);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, -10);
			t.orientation = Quat(Degs(-15), Degs(180), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(102));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
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
