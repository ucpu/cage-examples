#include <cage-core/assetsManager.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/profiling.h>
#include <cage-core/string.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

constexpr uint32 AssetsName = HashString("cage-tests/music/music.pack");

uint64 updateDelay = 0;
uint64 renderDelay = 0;
uint64 soundDelay = 0;

struct ShotComponent
{
	uint32 ttl = 20;
};

void shoot(const Transform &where)
{
	auto ents = engineEntities();
	Entity *e = ents->createAnonymous();
	e->value<TransformComponent>() = where;
	e->value<TransformComponent>().scale = 0.25;
	e->value<ShotComponent>();
	e->value<ModelComponent>().model = HashString("cage/models/fake.obj");
}

void controlInit()
{
	EntityManager *ents = engineEntities();
	ents->defineComponent(ShotComponent());

	{ // camera
		Entity *e = ents->create(1);
		e->value<TransformComponent>();
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 1;
		e->value<ListenerComponent>();
	}

	{ // box
		Entity *e = ents->create(2);
		e->value<TransformComponent>();
		e->value<ModelComponent>().model = HashString("cage/models/fake.obj");
		e->value<SoundComponent>().sound = HashString("cage-tests/music/PurplePlanet/RetroGamer.mp3");
		e->value<SoundComponent>().loop = true;
	}
}

void update()
{
	const uint64 time = engineControlTime();
	EntityManager *ents = engineEntities();

	{ // box
		Entity *e = ents->get(2);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(sin(Rads(time * 1e-6)) * 10, cos(Rads(time * 1e-6)) * 10, -20);
	}

	entitiesVisitor(
		[&](Entity *e, TransformComponent &t, ShotComponent &s)
		{
			if (s.ttl-- == 0)
				e->destroy();
			else
				t.position += t.orientation * Vec3(0, 0, -2);
		},
		ents, true);
	shoot(Transform(Vec3(-20, 0, -25), Quat(Degs(), Degs(-90), Degs())));

	{
		const ProfilingScope profiling("update delay");
		threadSleep(updateDelay);
	}

	{
		const ProfilingScope profiling("manual delay");
		if (engineWindow()->keyboardKey(32))
			threadSleep(1'000'000);
	}
}

void render()
{
	if (!renderDelay)
		return;
	const ProfilingScope profiling("render delay");
	threadSleep(renderDelay);
}

void soundUpdate()
{
	if (!soundDelay)
		return;
	const ProfilingScope profiling("sound delay");
	threadSleep(soundDelay);
}

void guiInit()
{
	Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
	auto _1 = g->alignment(Vec2(0));
	auto _2 = g->panel();
	auto _3 = g->scrollbars();
	auto _4 = g->verticalTable(2);

	static constexpr const char *names[] = { "control tick", "control delay", "graphics delay", "sound delay" };
	static constexpr const uint64 values[] = { 1000 / 20, 0, 0, 0 };
	static_assert(array_size(names) == array_size(values));
	for (uint32 i = 0; i < array_size(names); i++)
	{
		g->label().text(names[i]);
		g->input(values[i], i >= 1 ? 0 : 1, 1000)
			.event(inputFilter(
				[i](input::GuiValue v)
				{
					const GuiInputComponent &t = v.entity->value<GuiInputComponent>();
					if (!t.valid)
						return;
					CAGE_ASSERT(isDigitsOnly(t.value) && !t.value.empty());
					if (i == 0)
					{
						controlThread().updatePeriod(toUint32(t.value) * 1000);
					}
					else
					{
						static uint64 *data[] = { nullptr, &updateDelay, &renderDelay, &soundDelay };
						static_assert(array_size(data) == array_size(values));
						*(data[i]) = toUint32(t.value) * 1000;
					}
				}));
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(update);
		const auto renderListener = graphicsThread().graphics.listen(render);
		const auto soundListener = soundThread().sound.listen(soundUpdate);
		const auto guiInitListener = controlThread().initialize.listen(guiInit);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		engineWindow()->setMaximized();
		engineWindow()->title("timing frames");
		controlInit();
		engineAssets()->load(AssetsName);

		{
			Holder<StatisticsGui> statistics = newStatisticsGui();
			statistics->statisticsMode = StatisticsGuiModeEnum::Latest;
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
