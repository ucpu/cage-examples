#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assets.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/opengl.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/skeletons/skeletons.pack");

bool windowClose()
{
	engineStop();
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
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test engine skeleton");

		// entities
		entityManagerClass *ents = entities();
		{ // lemurs
			uint32 animations[] = { hashString("cage-tests/skeletons/lemur/lemur.x?AttackMelee"), hashString("cage-tests/skeletons/lemur/lemur.x?idle"), hashString("cage-tests/skeletons/lemur/lemur.x?run") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entityClass *e = ents->newEntity(1 + i);
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = hashString("cage-tests/skeletons/lemur/lemur.x");
				ENGINE_GET_COMPONENT(animatedSkeleton, s, e);
				s.name = animation;
				ENGINE_GET_COMPONENT(transform, t, e);
				t.position = vec3(i * 3 - 3.f, 0, 3);
				t.scale = 0.01;
				i++;
			}
		}
		{ // cylinders
			uint32 animations[] = { hashString("cage-tests/skeletons/cylinder/cylinder.x?bend"), hashString("cage-tests/skeletons/cylinder/cylinder.x?curve") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entityClass *e = ents->newEntity(10 + i);
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = hashString("cage-tests/skeletons/cylinder/cylinder.x");
				ENGINE_GET_COMPONENT(animatedSkeleton, s, e);
				s.name = animation;
				ENGINE_GET_COMPONENT(transform, t, e);
				t.position = vec3(i * 3 - 1.5f, 0, 0);
				t.scale = 0.5;
				i++;
			}
		}
		{ // monks
			uint32 animations[] = { hashString("cage-tests/skeletons/monk/monk.x?Attack1"), hashString("cage-tests/skeletons/monk/monk.x?Dance"), hashString("cage-tests/skeletons/monk/monk.x?Walk") };
			uint32 i = 0;
			for (uint32 animation : animations)
			{
				entityClass *e = ents->newEntity(20 + i);
				ENGINE_GET_COMPONENT(render, r, e);
				r.object = hashString("cage-tests/skeletons/monk/monk.object");
				ENGINE_GET_COMPONENT(animatedSkeleton, s, e);
				s.name = animation;
				ENGINE_GET_COMPONENT(transform, t, e);
				t.position = vec3(i * 3 - 3.f, 0, -3);
				i++;
			}
		}
		{ // floor
			entityClass *e = ents->newEntity(100);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/skeletons/floor/floor.obj");
			ENGINE_GET_COMPONENT(transform, t, e);
			(void)t;
		}
		{ // sun
			entityClass *e = ents->newEntity(101);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-50), degs(42), degs());
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			ENGINE_GET_COMPONENT(shadowmap, s, e);
			s.resolution = 4096;
			s.worldRadius = vec3(15, 15, 15);
		}
		{ // camera
			entityClass *e = ents->newEntity(102);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 5, 10);
			t.orientation = quat(degs(-10), degs(), degs());
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(0.1, 0.1, 0.1);
			c.near = 0.1;
			c.far = 100;
		}

		holder<cameraControllerClass> cameraController = newCameraController(ents->getEntity(102));
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.3;
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

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
