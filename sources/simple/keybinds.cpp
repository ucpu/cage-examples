#include <vector>

#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/keybinds.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/screamers/screamers.pack");

Holder<Keybind> keyTurnLeft = newKeybind({ .id = "turnLeft", .modes = KeybindModesFlags::Tick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'A' } }, input::KeyPress{ { .key = 263 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().orientation *= Quat({}, Degs(10), {});
		return false;
	});

Holder<Keybind> keyTurnRight = newKeybind({ .id = "turnRight", .modes = KeybindModesFlags::Tick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'D' } }, input::KeyPress{ { .key = 262 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().orientation *= Quat({}, Degs(-10), {});
		return false;
	});

Holder<Keybind> keyWalkForward = newKeybind({ .id = "walkForward", .modes = KeybindModesFlags::Tick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'W' } }, input::KeyPress{ { .key = 265 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(0, 0, -1);
		return false;
	});

Holder<Keybind> keyWalkBack = newKeybind({ .id = "walkBack", .modes = KeybindModesFlags::Tick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'S' } }, input::KeyPress{ { .key = 264 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(0, 0, 1);
		return false;
	});

Holder<Keybind> keyStrafeLeft = newKeybind({ .id = "strafeLeft", .modes = KeybindModesFlags::Tick }, input::KeyPress{ { .key = 'Q' } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(-1, 0, 0);
		return false;
	});

Holder<Keybind> keyStrafeRight = newKeybind({ .id = "strafeRight", .modes = KeybindModesFlags::Tick }, input::KeyPress{ { .key = 'E' } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(1, 0, 0);
		return false;
	});

Holder<Keybind> keyGrowBig = newKeybind({ .id = "growBig" }, input::MousePress{ { .buttons = MouseButtonsFlags::Left, .mods = ModifiersFlags::Ctrl } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().scale = 2;
		return false;
	});

Holder<Keybind> keyShrinkSmall = newKeybind({ .id = "shrinkSmall" }, input::MousePress{ { .buttons = MouseButtonsFlags::Right, .mods = ModifiersFlags::Ctrl } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().scale = 1;
		return false;
	});

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(keybindsDispatchTick);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("keybinds");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->createUnique();
			e->value<TransformComponent>().position = Vec3(0, 60, 30);
			e->value<TransformComponent>().orientation = Quat(Degs(-70), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
		}
		{ // skybox
			Entity *e = ents->createUnique();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("cage-tests/screamers/skybox.obj");
		}
		{ // sun
			Entity *e = ents->createUnique();
			e->value<TransformComponent>().orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 0.5;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 4096;
			s.directionalWorldSize = 50;
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
		}
		{ // monkey
			Entity *e = ents->create(1);
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("cage-tests/screamers/suzanne.blend");
		}

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
			auto _1 = g->alignment(Vec2());
			auto _2 = g->spoiler().text("Controls");
			auto _3 = g->scrollbars();
			keybindsGuiTable(+g);
		}

		engineAssets()->load(AssetsName);
		engineRun();
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
