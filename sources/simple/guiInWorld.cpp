#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>

#include <cage-simple/engine.h>
#include <cage-simple/statisticsGui.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/guiInWorld.h>

using namespace cage;

constexpr uint32 assetsName = HashString("cage-tests/cross/cross.pack");

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	Entity *e = engineEntities()->get(101);
	const uint64 time = engineControlTime();
	const Rads angle = Degs(time * 3e-6) + Degs(100);
	const Real dist = 10;
	e->value<TransformComponent>().position = Vec3(sin(angle) * dist, 0, cos(angle) * dist);
	e->value<TransformComponent>().orientation = Quat(Degs(), angle, Degs());
}

void makeGui(EntityManager *ents, const String &title)
{
	Holder<GuiBuilder> g = newGuiBuilder(ents);
	auto _1 = g->panel().text(title);
	auto _2 = g->verticalTable(3);
	for (uint32 i = 0; i < 9; i++)
		g->button().text(Stringizer() + i);
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
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		// window
		engineWindow()->title("gui in world");
		engineWindow()->setMaximized();

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, 10);
			t.orientation = Quat(Degs(), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.05;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // skybox
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/skybox.obj");
			e->value<TextureAnimationComponent>();
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 0);
			t.orientation = Quat(Degs(-60), randomAngle(), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.worldSize = Vec3(30);
			//e->value<RenderComponent>().object = HashString("cage-tests/cross/cross.obj");
		}
		{ // floor
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
			e->value<TransformComponent>().position = Vec3(0, -1.264425, 0);
		}
		{ // test cross
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = Vec3(10, 0, 10);
			e->value<RenderComponent>().object = HashString("cage-tests/cross/cross.obj");
		}
		{ // gui on floor
			Entity *e = ents->create(100);
			e->value<TransformComponent>().position = Vec3(0, -1.0, 0);
			e->value<TransformComponent>().orientation = Quat(Degs(70), Degs(180), Degs());
			e->value<TransformComponent>().scale = 1.3;
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
		}
		{ // circling gui
			Entity *e = ents->create(101);
			e->value<TransformComponent>().scale = 2;
			e->value<RenderComponent>().object = HashString("cage/model/fake.obj");
		}

		engineAssets()->add(assetsName);
		{
			Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(1));
			fpsCamera->mouseButton = MouseButtonsFlags::Right;
			fpsCamera->movementSpeed = 0.3;
			Holder<StatisticsGui> statistics = newStatisticsGui();
			Holder<GuiInWorld> floorGui = newGuiInWorld(GuiInWorldCreateConfig{ ents->get(100), ents->get(1) });
			makeGui(floorGui->guiEntities(), "floor gui");
			Holder<GuiInWorld> circlingGui = newGuiInWorld(GuiInWorldCreateConfig{ ents->get(101), ents->get(1) });
			makeGui(circlingGui->guiEntities(), "circling gui");
			engineRun();
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
