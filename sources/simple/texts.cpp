#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/texts.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/texts/texts.pack");

constexpr const char *FontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/fonts/ubuntu/regular.ttf",
	"cage/fonts/ubuntu/bold.ttf",
	"cage/fonts/ubuntu/italic.ttf",
	"cage/fonts/ubuntu/monospace.ttf",
};
constexpr const char *LabelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu Regular",
	"Ubuntu Bold",
	"Ubuntu Italic",
	"Ubuntu Mono",
};

constexpr uint32 fontsCount = array_size(FontNames);
static_assert(array_size(FontNames) == array_size(LabelTexts), "arrays must have same number of elements");

NoiseFunctionCreateConfig noiseInit(uint32 seed)
{
	NoiseFunctionCreateConfig cfg;
	cfg.seed = seed;
	cfg.type = NoiseTypeEnum::Cellular;
	cfg.frequency = 1e-7;
	return cfg;
}

void update()
{
	EntityManager *ents = engineEntities();

	{
		static Holder<NoiseFunction> noise1 = newNoiseFunction(noiseInit(42));
		static Holder<NoiseFunction> noise2 = newNoiseFunction(noiseInit(13));
		Entity *e = ents->get(10);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(noise1->evaluate(engineControlTime()) * 2, noise2->evaluate(engineControlTime()) * 2, 10);
		e->value<TextComponent>();
		e->value<TextValueComponent>().value = Stringizer() + t.position[0] + "|" + t.position[1] + "|" + t.position[2];
	}

	{
		Entity *e = ents->get(11);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(engineControlTime() * 1e-5), Degs());
	}
}

int main(int argc, char *args[])
{
	//detail::GuiTextFontDefault = HashString("cage-tests/gui/noto/universal.ttf");

	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("texts");

		// entities
		EntityManager *ents = engineEntities();
		{ // floor
			Entity *e = ents->create(1);
			e->value<ModelComponent>().model = HashString("cage-tests/skeletons/floor/floor.obj");
			e->value<TransformComponent>().position = Vec3(0, -5, 0);
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			e->value<LightComponent>().lightType = LightTypeEnum::Directional;
			e->value<ColorComponent>() = { Vec3(1), 3 };
		}
		{ // camera
			Entity *e = ents->create(3);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-10), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.02;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // text hello
			Entity *e = ents->create(11);
			e->value<TextComponent>().textId = HashString("short");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, -10);
			t.scale = 3;
		}
		{ // text long
			Entity *e = ents->createAnonymous();
			e->value<TextComponent>().textId = HashString("long/a");
			e->value<ColorComponent>() = { Vec3(1, 0, 0) };
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, -20);
		}
		{ // text lorem ipsum
			Entity *e = ents->createAnonymous();
			e->value<TextComponent>();
			e->value<TextValueComponent>().value = "Lorem ipsum dolor sit amet,\nconsectetur adipisici elit,\nsed eiusmod tempor incidunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat.";
			e->value<ColorComponent>() = { Vec3(0, 0, 1) };
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(10, 0, 0);
			t.orientation = Quat(Degs(), Degs(-90), Degs());
		}
		{ // text long paragprah
			Entity *e = ents->createAnonymous();
			e->value<TextComponent>();
			e->value<TextValueComponent>().value = "Za devatero horami a devatero řekami, v kouzelném kraji, kde se sny stávají skutečností, žilo bylo...\r\nKdo? Král, obyčejný chalupník, nebo snad kouzelná bytost? Jak má příběh pokračovat?";
			e->value<ColorComponent>() = { Vec3(0.6) };
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-10, 0, 0);
			t.orientation = Quat(Degs(), Degs(90), Degs());
		}
		{ // text params
			Entity *e = ents->create(10);
			e->value<TextComponent>().textId = HashString("params");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, 10);
			t.orientation = Quat(Degs(), Degs(180), Degs());
		}
		{ // fonts
			for (uint32 i = 0; i < fontsCount; i++)
			{
				Entity *e = ents->createAnonymous();
				e->value<TextComponent>().font = HashString(FontNames[i]);
				e->value<TextValueComponent>() = LabelTexts[i];
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(0, -3, 2.0 * i - fontsCount);
				t.orientation = Quat(Degs(-90), Degs(), Degs());
			}
		}
		{ // icon (czech)
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = Vec3(-10, 4, 0);
			e->value<TransformComponent>().orientation = Quat(Degs(), Degs(90), Degs());
			e->value<IconComponent>().icon = HashString("cage-tests/texts/czech.png");
		}
		{ // icon (english)
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = Vec3(10, 4, 0);
			e->value<TransformComponent>().orientation = Quat(Degs(), Degs(-90), Degs());
			e->value<IconComponent>().icon = HashString("cage-tests/texts/english.png");
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(3));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		if (randomChance() < 0.5)
			textsSetLanguages("cs_CZ;");

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
