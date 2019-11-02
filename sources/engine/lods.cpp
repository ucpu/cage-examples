#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/lods/lods.pack");

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
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("levels of details");

		// entities
		entityManager *ents = entities();
		{ // cube
			entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/lods/cube.object");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(-8, 0, 0);
		}
		{ // sphere
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/lods/sphere.object");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(8, 0, 0);
		}
		{ // floor
			entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/lods/floor.object");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, -5, 0);
		}
		{ // sun
			entity *e = ents->create(9);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-60), degs(20), degs());
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			CAGE_COMPONENT_ENGINE(shadowmap, s, e);
			s.resolution = 1024;
			s.worldSize = vec3(20, 20, 20);
		}
		{ // camera
			entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 3, 30);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.5;
			c.near = 0.1;
			c.far = 200;
		}

		holder<fpsCamera> fpsCamera = newFpsCamera(ents->get(10));
		fpsCamera->mouseButton = mouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 1;
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
