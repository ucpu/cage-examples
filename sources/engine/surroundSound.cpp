#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

const uint32 assetsName = hashString("cage-tests/logo/logo.ogg");

bool windowClose()
{
	engineStop();
	return false;
}

void controlInit()
{
	entityManager *ents = entities();

	{ // camera
		entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(camera, c, e);
		c.ambientLight = vec3(1, 1, 1);
		c.cameraType = cameraTypeEnum::Orthographic;
		c.camera.orthographicSize = vec2(50, 50);
		c.near = -5;
		c.far = 5;
	}

	{ // listener
		entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.orientation = quat(degs(90), degs(), degs());
		CAGE_COMPONENT_ENGINE(render, r, e);
		r.object = 1;
		CAGE_COMPONENT_ENGINE(listener, l, e);
		l.attenuation = vec3(0, 0.1, 0.005);
	}

	static const vec3 boxPositions[] = { vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0), vec3(1, 1, 0) };
	for (uint32 i = 0; i < sizeof(boxPositions) / sizeof(boxPositions[0]); i++)
	{ // box
		entity *e = ents->create(10 + i);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.position = boxPositions[i] * 30;
		CAGE_COMPONENT_ENGINE(render, r, e);
		r.object = 2;
		CAGE_COMPONENT_ENGINE(voice, s, e);
		s.name = assetsName;
	}
}

bool update()
{
	entityManager *ents = entities();
	{ // listener
		entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		ivec2 cursor = window()->mousePosition();
		ivec2 resolution = window()->resolution();
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
		holder<logger> log1 = newLogger();
		log1->format.bind <logFormatConsole>();
		log1->output.bind <logOutputStdOut>();

		configSetBool("cage-client.engine.renderMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(window()->events.windowClose);

		window()->setWindowed();
		window()->windowedSize(ivec2(600, 600));
		window()->title("surround sound");
		controlInit();

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
