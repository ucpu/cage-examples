#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/sound.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

holder<sourceClass> toneSource;
holder<busClass> toneBus;

bool windowClose()
{
	engineStop();
	return false;
}

void controlInit()
{
	entityManagerClass *ents = entities();

	{ // camera
		entityClass *e = ents->create(1);
		ENGINE_GET_COMPONENT(transform, t, e);
		(void)t;
		ENGINE_GET_COMPONENT(camera, c, e);
		c.ambientLight = vec3(1, 1, 1);
		c.cameraType = cameraTypeEnum::Orthographic;
		c.camera.orthographicSize = vec2(50, 50);
		c.near = -5;
		c.far = 5;
	}

	{ // listener
		entityClass *e = ents->create(2);
		ENGINE_GET_COMPONENT(render, r, e);
		r.object = 1;
		ENGINE_GET_COMPONENT(listener, l, e);
		l.attenuation = vec3(1, 0.01, 0);
		l.dopplerEffect = true;
	}

	{ // moving voice
		entityClass *e = ents->create(3);
		ENGINE_GET_COMPONENT(transform, t, e);
		(void)t;
		ENGINE_GET_COMPONENT(render, r, e);
		r.object = 2;
		ENGINE_GET_COMPONENT(voice, s, e);
		s.name = hashString("cage/sound/logo.ogg");
	}
}

bool soundInit()
{
	toneBus = newBus(sound());
	toneSource = newSource(sound());
	toneSource->setDataTone();
	toneSource->addOutput(toneBus.get());
	return false;
}

bool soundFinish()
{
	toneSource.clear();
	toneBus.clear();
	return false;
}

bool update()
{
	uint64 time = currentControlTime();
	entityManagerClass *ents = entities();
	vec3 box;
	{ // moving voice
		entityClass *e = ents->get(3);
		ENGINE_GET_COMPONENT(transform, t, e);
		static const uint64 duration = 8000000;
		bool odd = (time / duration) % 2 == 1;
		real x = ((real)time % duration) / duration;
		if (odd)
			x = 1 - x;
		box = t.position = vec3(100 * (x - 0.5), 10, 0);
		if (toneBus)
		{
			ENGINE_GET_COMPONENT(voice, v, e);
			v.input = toneBus.get();
			v.name = 0;
		}
	}
	box = box.normalize();
	{ // listener
		entityClass *e = ents->get(2);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.orientation = quat(degs(90), degs(), degs()) * quat(degs(), aTan2(box[0], box[1]) - degs(90), degs());
	}
	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		configSetBool("cage-client.engine.renderMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), soundInit, soundThread().initialize);
		GCHL_GENERATE((), soundFinish, soundThread().finalize);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
#undef GCHL_GENERATE

		window()->setWindowed();
		window()->windowedSize(pointStruct(600, 600));
		window()->title("doppler [WIP]");
		controlInit();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
