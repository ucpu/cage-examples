#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/sound.h>
#include <cage-engine/engine.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

Holder<SoundSource> toneSource;
Holder<MixingBus> toneBus;

bool windowClose()
{
	engineStop();
	return false;
}

void controlInit()
{
	EntityManager *ents = entities();

	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientLight = vec3(1, 1, 1);
		c.cameraType = CameraTypeEnum::Orthographic;
		c.camera.orthographicSize = vec2(50, 50);
		c.near = -5;
		c.far = 5;
	}

	{ // listener
		Entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage/mesh/fake.obj");
		CAGE_COMPONENT_ENGINE(Listener, l, e);
		l.attenuation = vec3(1, 0.01, 0);
		l.dopplerEffect = true;
	}

	{ // moving voice
		Entity *e = ents->create(3);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		(void)t;
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage/mesh/fake.obj");
		CAGE_COMPONENT_ENGINE(Sound, s, e);
		s.name = HashString("cage/sound/logo.ogg");
	}
}

bool soundInit()
{
	toneBus = newMixingBus(sound());
	toneSource = newSoundSource(sound());
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
	EntityManager *ents = entities();
	vec3 box;
	{ // moving voice
		Entity *e = ents->get(3);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		static const uint64 duration = 8000000;
		bool odd = (time / duration) % 2 == 1;
		real x = ((real)time % duration) / duration;
		if (odd)
			x = 1 - x;
		box = t.position = vec3(100 * (x - 0.5), 10, 0);
		if (toneBus)
		{
			CAGE_COMPONENT_ENGINE(Sound, v, e);
			v.input = toneBus.get();
			v.name = 0;
		}
	}
	box = normalize(box);
	{ // listener
		Entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(90), degs(), degs()) * quat(degs(), atan2(box[0], box[1]) - degs(90), degs());
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
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), soundInit, soundThread().initialize);
		GCHL_GENERATE((), soundFinish, soundThread().finalize);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
#undef GCHL_GENERATE

		window()->setWindowed();
		window()->windowedSize(ivec2(600, 600));
		window()->title("doppler [WIP]");
		controlInit();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
