#include <vector>

#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/files.h>
#include <cage-core/hashString.h>
#include <cage-core/ini.h>
#include <cage-core/logger.h>
#include <cage-core/string.h>
#include <cage-core/stringLiteral.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/keybinds.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/screamers/screamers.pack");

struct Printer
{
	Stringizer str;

	CAGE_FORCE_INLINE String finishName(String s) { return replace(trim(s), " ", " + "); }

	CAGE_FORCE_INLINE void printImpl(const input::privat::BaseKey &input) { str + finishName(Stringizer() + getModifiersNames(input.mods) + " " + getKeyName(input.key)); }

	CAGE_FORCE_INLINE void printImpl(const input::privat::BaseMouse &input) { str + finishName(Stringizer() + getModifiersNames(input.mods) + " " + getButtonsNames(input.buttons)); }

	CAGE_FORCE_INLINE void printImpl(const input::MouseWheel &input) { str + finishName(getModifiersNames(input.mods)); }

	template<class T, StringLiteral Name>
	CAGE_FORCE_INLINE void print(const GenericInput &input)
	{
		if (input.has<T>())
		{
			str + Name.value + ": ";
			printImpl(input.get<T>());
		}
	}

	CAGE_FORCE_INLINE String print(const GenericInput &input)
	{
		print<input::KeyPress, "key press">(input);
		//print<input::KeyRepeat, "key repeat">(input);
		print<input::KeyRelease, "key release">(input);
		print<input::MousePress, "mouse press">(input);
		print<input::MouseDoublePress, "mouse double press">(input);
		print<input::MouseRelease, "mouse release">(input);
		print<input::MouseWheel, "mouse wheel">(input);
		return str;
	}
};

String toString(const GenericInput &input)
{
	return Printer().print(input);
}

const auto inputEventListener = engineEvents().listen(
	[](const GenericInput &input)
	{
		const String s = toString(input);
		if (!s.empty())
			CAGE_LOG(SeverityEnum::Info, "input", s);
	});

Holder<Keybind> keyTurnLeft = newKeybind({ .id = "turnLeft", .modes = KeybindModesFlags::EngineTick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'A' } }, input::KeyPress{ { .key = 263 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().orientation *= Quat({}, Degs(10), {});
		return false;
	});

Holder<Keybind> keyTurnRight = newKeybind({ .id = "turnRight", .modes = KeybindModesFlags::EngineTick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'D' } }, input::KeyPress{ { .key = 262 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().orientation *= Quat({}, Degs(-10), {});
		return false;
	});

Holder<Keybind> keyWalkForward = newKeybind({ .id = "walkForward", .modes = KeybindModesFlags::EngineTick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'W' } }, input::KeyPress{ { .key = 265 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(0, 0, -1);
		return false;
	});

Holder<Keybind> keyWalkBack = newKeybind({ .id = "walkBack", .modes = KeybindModesFlags::EngineTick }, std::vector<GenericInput>{ input::KeyPress{ { .key = 'S' } }, input::KeyPress{ { .key = 264 } } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(0, 0, 1);
		return false;
	});

Holder<Keybind> keyStrafeLeft = newKeybind({ .id = "strafeLeft", .modes = KeybindModesFlags::EngineTick }, input::KeyPress{ { .key = 'Q' } },
	[](const GenericInput &)
	{
		Entity *e = engineEntities()->get(1);
		e->value<TransformComponent>().position += e->value<TransformComponent>().orientation * Vec3(-1, 0, 0);
		return false;
	});

Holder<Keybind> keyStrafeRight = newKeybind({ .id = "strafeRight", .modes = KeybindModesFlags::EngineTick }, input::KeyPress{ { .key = 'E' } },
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

Holder<Keybind> keyApplause = newKeybind(KeybindCreateConfig{ .id = "applause", .devices = KeybindDevicesFlags::Modifiers }, input::KeyPress{ { .mods = ModifiersFlags::Shift } }, [](const GenericInput &) { return false; });

String filename(uint32 i)
{
	return Stringizer() + "keybinds_" + i + ".ini";
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
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
			e->value<ModelComponent>().model = HashString("cage-tests/screamers/skybox.obj");
		}
		{ // sun
			Entity *e = ents->createUnique();
			e->value<TransformComponent>().orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			e->value<LightComponent>().lightType = LightTypeEnum::Directional;
			e->value<ColorComponent>().intensity = 0.5;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 4096;
			s.cascadesPaddingDistance = 50;
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<TransformComponent>();
			e->value<ModelComponent>().model = HashString("scenes/common/ground.obj");
		}
		{ // monkey
			Entity *e = ents->create(1);
			e->value<TransformComponent>();
			e->value<ModelComponent>().model = HashString("cage-tests/screamers/suzanne.blend");
		}

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
			auto _1 = g->alignment(Vec2());
			auto _2 = g->spoiler().text("Controls");
			auto _3 = g->scrollbars();
			auto _4 = g->column();
			keybindsGuiTable(+g);
			{
				auto _ = g->horizontalTable(2);
				for (uint32 i = 0; i < 3; i++)
				{
					g->button()
						.text(Stringizer() + "Save " + i)
						.event(
							[i](const GenericInput &) -> bool
							{
								keybindsExport()->exportFile(filename(i));
								return true;
							});
					g->button()
						.text(Stringizer() + "Load " + i)
						.event(
							[i](const GenericInput &) -> bool
							{
								Holder<Ini> ini = newIni();
								ini->importFile(filename(i));
								keybindsImport(+ini, true);
								return true;
							})
						.update([i](Entity *e) { e->value<GuiWidgetStateComponent>().disabled = !pathIsFile(filename(i)); });
				}
			}
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
