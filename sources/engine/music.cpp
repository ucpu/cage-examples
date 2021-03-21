#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/enumerate.h>
#include <cage-core/string.h>

#include <cage-engine/window.h>
#include <cage-engine/engine.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>

using namespace cage;

constexpr uint32 assetsName = HashString("cage-tests/music/music.pack");

void windowClose()
{
	engineStop();
}

void init()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(0, 1.7, 0);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientColor = vec3(1);
		c.ambientIntensity = 0.2;
		c.ambientDirectionalColor = vec3(1);
		c.ambientDirectionalIntensity = 0.2;
		c.near = 0.1;
		c.far = 500;
		CAGE_COMPONENT_ENGINE(Listener, l, e);
		l.rolloffFactor = 0.05;
	}

	{ // sun
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(-90), {}, {});
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Directional;
		CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
		s.resolution = 4096;
		s.worldSize = vec3(200);
	}

	{ // floor
		Entity *e = ents->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.scale = 200 / 8;
		CAGE_COMPONENT_ENGINE(Render, r, e);
		r.object = HashString("cage-tests/lods/floor.object");
	}

	constexpr const char *names[] = { "BigDayOut.mp3", "Civilisation.mp3", "FunkCity.mp3", "IntoBattle.mp3", "RetroGamer.mp3", "TrueFaith.mp3" };
	for (auto it : enumerate(names))
	{ // sound source
		{
			Entity *e = ents->create(numeric_cast<uint32>(it.index) + 100);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.scale = 2;
			CAGE_COMPONENT_ENGINE(Sound, s, e);
			s.name = HashString(string(stringizer() + "cage-tests/music/PurplePlanet/" + *it));
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.color = vec3(1);
			r.object = HashString("scenes/common/lightbulb.obj");
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.intensity = 2;
		}
		{
			Entity *e = ents->create(numeric_cast<uint32>(it.index) + 200);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.scale = 0.5;
			CAGE_COMPONENT_ENGINE(Text, l, e);
			string n = *it;
			l.value = split(n, ".");
		}
	}
}

void update()
{
	EntityManager *ents = engineEntities();

	const vec3 cameraPos = [&]() {
		Entity *e = ents->get(1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		return t.position;
	}();

	vec3 positions[] = {
		vec3(-100, 1, -100),
		vec3(0, 1, -100),
		vec3(100, 1, -100),
		vec3(-100, 1, 0),
		vec3(100, 2, 0),
		vec3(0, 1, 100),
	};

	{
		const uint64 t = engineControlTime();

		rads a = rads(t * 3e-7);
		real r = 10;
		positions[3] = vec3(positions[3][0] + sin(a) * r, positions[3][1], positions[3][2] + cos(a) * r);

		a = rads(t * 5e-7);
		r = 2;
		positions[4] = vec3(positions[4][0], positions[4][1] + sin(a) * r, positions[4][2] + cos(a) * r);

		a = rads(t * 2e-7);
		r = 20;
		positions[5] = vec3(positions[5][0] + cos(a) * r, positions[5][1], positions[5][2]);
	}

	for (auto it : enumerate(positions))
	{
		{
			Entity *e = ents->get(numeric_cast<uint32>(it.index) + 100);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = *it;
		}
		{
			Entity *e = ents->get(numeric_cast<uint32>(it.index) + 200);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = *it + vec3(0, 0.25, 0);
			if (distance(cameraPos, t.position) > 0.1)
				t.orientation = quat(t.position - cameraPos, vec3(0, 1, 0));
		}
	}
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
		EventListener<void()> updateListener;
		updateListener.bind<&update>();
		updateListener.attach(controlThread().update);
		EventListener<void()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(engineWindow()->events.windowClose);

		engineWindow()->setMaximized();
		engineWindow()->title("music");
		init();

		engineAssets()->add(assetsName);
		{
			Holder<EngineProfiling> profiling = newEngineProfiling();
			Holder<FpsCamera> fps = newFpsCamera(engineEntities()->get(1));
			fps->mouseButton = MouseButtonsFlags::Left;
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
