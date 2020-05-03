#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>

#include <cage-engine/window.h>
#include <cage-engine/sound.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

const uint32 assetsName = HashString("cage-tests/sounds/sounds.pack");
Holder<SoundSource> toneSource;
Holder<MixingBus> toneBus;

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
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientColor = vec3(1);
		c.ambientIntensity = 0.5;
		c.ambientDirectionalColor = vec3(1);
		c.ambientDirectionalIntensity = 0.5;
		c.cameraType = CameraTypeEnum::Orthographic;
		c.camera.orthographicSize = vec2(50, 50);
		c.near = -50;
		c.far = 50;
	}

	{ // listener
		Entity *e = ents->create(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 2;
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/sounds/listener.obj");
		CAGE_COMPONENT_ENGINE(Listener, l, e);
		l.attenuation = vec3(0, 0.01, 0);
		l.dopplerEffect = true;
	}

	{ // moving voice
		Entity *e = ents->create(3);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-90), degs(), degs());
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/sounds/speaker.obj");
		CAGE_COMPONENT_ENGINE(Sound, s, e);
		s.name = HashString("cage-tests/logo/logo.ogg");
	}
}

bool soundInit()
{
	toneBus = newMixingBus(engineSound());
	toneSource = newSoundSource(engineSound());
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
	uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();
	vec3 box;
	{ // moving voice
		Entity *e = ents->get(3);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		static const uint64 duration = 8000000;
		bool odd = (time / duration) % 2 == 1;
		real x = ((real)time % duration) / duration;
		if (odd)
			x = 1 - x;
		box = t.position = vec3(100 * (x - 0.5), 5, 0);
		/*
		if (toneBus)
		{
			CAGE_COMPONENT_ENGINE(Sound, v, e);
			v.input = toneBus.get();
			v.name = 0;
		}
		*/
	}
	box = normalize(box);
	{ // listener
		Entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(box, vec3(0, 0, 1), false);
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
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> FUNC##Listener; FUNC##Listener.bind<&FUNC>(); FUNC##Listener.attach(EVENT);
		GCHL_GENERATE((), soundInit, soundThread().initialize);
		GCHL_GENERATE((), soundFinish, soundThread().finalize);
		GCHL_GENERATE((), update, controlThread().update);
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
#undef GCHL_GENERATE

		engineWindow()->setWindowed();
		engineWindow()->windowedSize(ivec2(600, 600));
		engineWindow()->title("doppler [WIP]");
		controlInit();

		engineAssets()->add(assetsName);
		{
			Holder<EngineProfiling> profiling = newEngineProfiling();
			engineStart();
		}
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
