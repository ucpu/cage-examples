#include <cage-core/assetManager.h>
#include <cage-core/color.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/mazetd/mazetd.pack");

void update()
{
	static uint64 timeToChange = 0;
	if (engineControlTime() < timeToChange)
		return;
	EntityManager *ents = engineEntities();
	Entity *e = ents->get(1);
	CameraComponent &c = e->value<CameraComponent>();
	c.sceneMask = (1u << randomRange(1u, 4u)) + 1;
	timeToChange = engineControlTime() + 1000000;
}

void generatePieces()
{
	static constexpr uint32 names[] = {
		HashString("cage-tests/mazetd/buildings/augment-fire.object"),
		HashString("cage-tests/mazetd/buildings/augment-magic.object"),
		HashString("cage-tests/mazetd/buildings/augment-poison.object"),
		HashString("cage-tests/mazetd/buildings/augment-water.object"),
		HashString("cage-tests/mazetd/buildings/bonus-damage.object"),
		HashString("cage-tests/mazetd/buildings/bonus-firingRange.object"),
		HashString("cage-tests/mazetd/buildings/bonus-firingRate.object"),
		HashString("cage-tests/mazetd/buildings/bonus-intenseDot.object"),
		HashString("cage-tests/mazetd/buildings/bonus-manaDiscount.object"),
		HashString("cage-tests/mazetd/buildings/bonus-splashRadius.object"),
		HashString("cage-tests/mazetd/buildings/mana-capacitor.object"),
		HashString("cage-tests/mazetd/buildings/mana-collector-snow.object"),
		HashString("cage-tests/mazetd/buildings/mana-collector-sun.object"),
		HashString("cage-tests/mazetd/buildings/mana-collector-water.object"),
		HashString("cage-tests/mazetd/buildings/mana-collector-wind.object"),
		HashString("cage-tests/mazetd/buildings/mana-relay.object"),
		HashString("cage-tests/mazetd/buildings/targeting-back.object"),
		HashString("cage-tests/mazetd/buildings/targeting-closest.object"),
		HashString("cage-tests/mazetd/buildings/targeting-farthest.object"),
		HashString("cage-tests/mazetd/buildings/targeting-front.object"),
		HashString("cage-tests/mazetd/buildings/targeting-strongest.object"),
		HashString("cage-tests/mazetd/buildings/targeting-weakest.object"),
		HashString("cage-tests/mazetd/buildings/tower-heavy.object"),
		HashString("cage-tests/mazetd/buildings/tower-light.object"),
		HashString("cage-tests/mazetd/buildings/tower-medium.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
		HashString("cage-tests/mazetd/buildings/wall.object"),
	};

	const auto &pick = [&]() { return randomRange(std::size_t(0), sizeof(names) / sizeof(names[0])); };

	EntityManager *ents = engineEntities();
	for (sint32 y = -5; y < 6; y++)
	{
		for (sint32 x = -5; x < 6; x++)
		{
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = names[pick()];
			if (randomChance() < 0.5)
				e->value<RenderComponent>().sceneMask = 1u << randomRange(1u, 4u);
			e->value<TextureAnimationComponent>();
			e->value<TransformComponent>().position = Vec3(x, 0, y) * 1.5;
		}
	}

	// colored lights
	for (uint32 i = 0; i < 3; i++)
	{
		Entity *e = ents->createUnique();
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(-50), Degs(90 * i), Degs());
		t.position = Vec3(0, 2, 0);
		LightComponent &l = e->value<LightComponent>();
		l.lightType = LightTypeEnum::Directional;
		l.color = Vec3(i == 0, i == 1, i == 2);
		l.intensity = 2;
		l.sceneMask = 1u << (i + 1);
		ShadowmapComponent &s = e->value<ShadowmapComponent>();
		s.resolution = 2048;
		s.worldSize = Vec3(15);
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
		const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("mazetd buildings (test scene masks)");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 5, 5);
			e->value<TransformComponent>().orientation = Quat(Degs(-45), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->createUnique();
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(270), Degs());
			t.position = Vec3(0, 2, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 1;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(15);
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/skeletons/floor/floor.obj");
			e->value<TransformComponent>().position = Vec3();
		}
		generatePieces();

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->load(assetsName);
		engineRun();
		engineAssets()->unload(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
