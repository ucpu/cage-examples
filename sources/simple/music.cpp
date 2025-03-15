#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/enumerate.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/string.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

constexpr uint32 AssetsName = HashString("cage-tests/music/music.pack");

void init()
{
	EntityManager *ents = engineEntities();

	{ // camera
		Entity *e = ents->create(1);
		e->value<TransformComponent>().position = Vec3(0, 1.7, 0);
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 0.3;
		c.near = 0.1;
		c.far = 500;
		e->value<ListenerComponent>();
	}

	{ // sun
		Entity *e = ents->createUnique();
		e->value<TransformComponent>().orientation = Quat(Degs(-90), {}, {});
		e->value<LightComponent>().lightType = LightTypeEnum::Directional;
		ShadowmapComponent &s = e->value<ShadowmapComponent>();
		s.resolution = 4096;
		s.directionalWorldSize = 200;
	}

	{ // floor
		Entity *e = ents->createAnonymous();
		e->value<ModelComponent>().model = HashString("cage-tests/lods/floor.object");
		e->value<TransformComponent>().position = Vec3(0, -5, 0);
		e->value<TransformComponent>().scale = 10;
	}

	constexpr const char *names[] = { "BigDayOut.mp3", "Civilisation.mp3", "FunkCity.mp3", "IntoBattle.mp3", "RetroGamer.mp3", "TrueFaith.mp3" };
	for (auto it : enumerate(names))
	{ // sound source
		{
			Entity *e = ents->create(numeric_cast<uint32>(it.index) + 100);
			e->value<TransformComponent>().scale = 2;
			e->value<SoundComponent>().sound = HashString(String(Stringizer() + "cage-tests/music/PurplePlanet/" + *it));
			e->value<SoundComponent>().loop = true;
			e->value<SoundComponent>().attenuation = SoundAttenuationEnum::InverseSquare;
			e->value<SoundComponent>().minDistance = 10;
			e->value<ModelComponent>().model = HashString("scenes/common/lightbulb.obj");
			e->value<LightComponent>();
			e->value<ColorComponent>().intensity = 2;
		}
		{
			Entity *e = ents->create(numeric_cast<uint32>(it.index) + 200);
			e->value<TransformComponent>().scale = 0.5;
			e->value<TextComponent>();
			String n = *it;
			e->value<TextValueComponent>() = split(n, ".");
		}
	}
}

void update()
{
	EntityManager *ents = engineEntities();

	const Vec3 cameraPos = [&]()
	{
		Entity *e = ents->get(1);
		TransformComponent &t = e->value<TransformComponent>();
		return t.position;
	}();

	Vec3 positions[] = {
		Vec3(-100, 1, -100),
		Vec3(0, 1, -100),
		Vec3(100, 1, -100),
		Vec3(-100, 1, 0),
		Vec3(100, 2, 0),
		Vec3(0, 1, 100),
	};

	{
		const uint64 t = engineControlTime();

		Rads a = Rads(t * 3e-7);
		Real r = 10;
		positions[3] = Vec3(positions[3][0] + sin(a) * r, positions[3][1], positions[3][2] + cos(a) * r);

		a = Rads(t * 5e-7);
		r = 2;
		positions[4] = Vec3(positions[4][0], positions[4][1] + sin(a) * r, positions[4][2] + cos(a) * r);

		a = Rads(t * 2e-7);
		r = 20;
		positions[5] = Vec3(positions[5][0] + cos(a) * r, positions[5][1], positions[5][2]);
	}

	for (auto it : enumerate(positions))
	{
		{
			Entity *e = ents->get(numeric_cast<uint32>(it.index) + 100);
			e->value<TransformComponent>().position = *it;
		}
		{
			Entity *e = ents->get(numeric_cast<uint32>(it.index) + 200);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = *it + Vec3(0, 0.4, 0);
			if (distance(cameraPos, t.position) > 0.1)
				t.orientation = Quat(t.position - cameraPos, Vec3(0, 1, 0));
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("music");
		init();

		engineAssets()->load(AssetsName);
		{
			Holder<StatisticsGui> statistics = newStatisticsGui();
			Holder<FpsCamera> fps = newFpsCamera(engineEntities()->get(1));
			fps->mouseButton = MouseButtonsFlags::Left;
			engineRun();
		}
		engineAssets()->unload(AssetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
