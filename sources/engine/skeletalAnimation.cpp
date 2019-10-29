#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/cameraController.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

static const uint32 assetsName = hashString("cage-tests/skeletons/skeletons.pack");

bool windowClose()
{
	engineStop();
	return false;
}

bool keyPress(uint32, uint32, modifiersFlags modifiers)
{
	return false;
}

bool update()
{
	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((uint32, uint32, modifiersFlags), keyPress, window()->events.keyPress);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("skeletal animation");

		// entities
		entityManager *ents = entities();
		{ // lemurs
			uint32 animations[] = { hashString("cage-tests/skeletons/lemur/lemur.x?AttackMelee"), hashString("cage-tests/skeletons/lemur/lemur.x?idle"), hashString("cage-tests/skeletons/lemur/lemur.x?run") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entity *e = ents->create(1 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/lemur/lemur.x");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = animation;
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 3);
				i++;
			}
			{ // no animation
				entity *e = ents->create(1 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/lemur/lemur.x");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 3);
				i++;
			}
			{ // scaled
				entity *e = ents->create(1 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/lemur/lemur.x");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = hashString("cage-tests/skeletons/lemur/lemur.x?idle");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 3);
				t.scale = 1.5;
				i++;
			}
#if (0)
			// performance
			for (sint32 i = 0; i < 32 * 32; i++)
			{
				entity *e = ents->createAnonymous();
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/lemur/lemur.x");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = animations[1];
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3((i % 32) * 2 - 32, 0, (i / 32) * 2 - 70);
			}
#endif
		}
		{ // cylinders
			uint32 animations[] = { hashString("cage-tests/skeletons/cylinder/cylinder.x?bend"), hashString("cage-tests/skeletons/cylinder/cylinder.x?curve") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entity *e = ents->create(10 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/cylinder/cylinder.x");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = animation;
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 0);
				i++;
			}
			{ // no animation set
				entity *e = ents->create(10 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/cylinder/cylinder.x");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 0);
				i++;
			}
			{ // non-existent animation
				entity *e = ents->create(10 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/cylinder/cylinder.x");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = hashString("cage-tests/skeletons/cylinder/cylinder.x?non-existent");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 0);
				i++;
			}
			{ // non-existent object
				entity *e = ents->create(10 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/cylinder/non-existent.x");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, 0);
				i++;
			}
		}
		{ // monks
			uint32 animations[] = { hashString("cage-tests/skeletons/monk/monk.x?Attack1"), hashString("cage-tests/skeletons/monk/monk.x?Dance"), hashString("cage-tests/skeletons/monk/monk.x?Die") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entity *e = ents->create(20 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/monk/monk.object");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = animation;
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, -3);
				i++;
			}
			{ // no animation
				entity *e = ents->create(20 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/monk/monk.object");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, -3);
				i++;
			}
			{ // rotated
				entity *e = ents->create(20 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = hashString("cage-tests/skeletons/monk/monk.object");
				CAGE_COMPONENT_ENGINE(skeletalAnimation, s, e);
				s.name = hashString("cage-tests/skeletons/monk/monk.x?Dance");
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 - 6.f, 0, -3);
				t.orientation = quat(degs(), degs(randomRange(45, 270)), degs());
				i++;
			}
		}
		{ // gimbals/spawners
			uint32 i = 0;
			uint32 objects[] = { hashString("cage-tests/skeletons/gimbal/spawner_x.object"), hashString("cage-tests/skeletons/gimbal/spawner_fbx.object") };
			for (uint32 object : objects)
			{
				entity *e = ents->create(30 + i);
				CAGE_COMPONENT_ENGINE(render, r, e);
				r.object = object;
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(i * 3 + 1.5f, 1, 0);
				t.scale = 0.6;
				i++;
			}
		}
		{ // floor
			entity *e = ents->create(100);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/skeletons/floor/floor.obj");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			(void)t;
		}
		{ // sun
			entity *e = ents->create(101);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(3);
			CAGE_COMPONENT_ENGINE(shadowmap, s, e);
			s.resolution = 2048;
			s.worldSize = vec3(12, 12, 10);
		}
		{ // camera
			entity *e = ents->create(102);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 5, 10);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(0.02);
			c.near = 0.1;
			c.far = 100;
			c.effects = cameraEffectsFlags::CombinedPass;
		}

		holder<cameraController> cameraCtrl = newCameraController(ents->get(102));
		cameraCtrl->mouseButton = mouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		holder<engineProfiling> engineProfiling = newEngineProfiling();

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
