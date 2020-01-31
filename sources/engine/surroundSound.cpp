#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>

#include <cage-engine/window.h>
#include <cage-engine/engine.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

const uint32 assetsName = HashString("cage-tests/sounds/sounds.pack");

bool windowClose()
{
	engineStop();
	return false;
}

void controlInit()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientLight = vec3(0.5);
		c.ambientDirectionalLight = vec3(0.5);
		c.cameraType = CameraTypeEnum::Orthographic;
		c.camera.orthographicSize = vec2(50, 50);
		c.near = -50;
		c.far = 50;
	}

	{ // listener
		Entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 2;
		t.orientation = quat(degs(90), degs(), degs());
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/sounds/listener.obj");
		CAGE_COMPONENT_ENGINE(Listener, l, e);
		l.attenuation = vec3(0, 0.01, 0.0);
	}

	static const vec3 boxPositions[] = { vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0), vec3(1, 1, 0) };
	for (uint32 i = 0; i < sizeof(boxPositions) / sizeof(boxPositions[0]); i++)
	{ // box
		Entity *e = ents->create(10 + i);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-90), degs(), degs());
		t.position = boxPositions[i] * 25;
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/sounds/speaker.obj");
		CAGE_COMPONENT_ENGINE(Sound, s, e);
		s.name = HashString("cage-tests/logo/logo.ogg");
	}
}

bool update()
{
	EntityManager *ents = engineEntities();
	{ // listener
		Entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		ivec2 cursor = engineWindow()->mousePosition();
		ivec2 resolution = engineWindow()->resolution();
		vec3 cur = vec3((real)cursor.x / (real)resolution.x, (real)1 - (real)cursor.y / (real)resolution.y, 0);
		t.position = 50 * (cur * 2 - 1) * vec3(1, 1, 0);
	}
	return false;
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
		EventListener<bool()> updateListener;
		updateListener.bind<&update>();
		updateListener.attach(controlThread().update);
		EventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(engineWindow()->events.windowClose);

		engineWindow()->setWindowed();
		engineWindow()->windowedSize(ivec2(600, 600));
		engineWindow()->title("surround sound");
		controlInit();

		engineAssets()->add(assetsName);
		engineStart();
		engineAssets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
