#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/macros.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/texts/texts.pack");

constexpr const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/Ubuntu-R.ttf",
};
constexpr const char *labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu",
};

constexpr uint32 fontsCount = sizeof(fontNames) / sizeof(fontNames[0]);
static_assert(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), "arrays must have same number of elements");

bool windowClose()
{
	engineStop();
	return false;
}

NoiseFunctionCreateConfig noiseInit(uint32 seed)
{
	NoiseFunctionCreateConfig cfg;
	cfg.seed = seed;
	cfg.type = NoiseTypeEnum::Cellular;
	cfg.frequency = 1e-7;
	return cfg;
}

bool update()
{
	EntityManager *ents = engineEntities();

	{
		static Holder<NoiseFunction> noise1 = newNoiseFunction(noiseInit(42));
		static Holder<NoiseFunction> noise2 = newNoiseFunction(noiseInit(13));
		Entity *e = ents->get(10);
		TransformComponent &t = e->value<TransformComponent>();
		t.position = Vec3(noise1->evaluate(engineControlTime()) * 2, noise2->evaluate(engineControlTime()) * 2, 10);
		TextComponent &r = e->value<TextComponent>();
		r.value = Stringizer() + t.position[0] + "|" + t.position[1] + "|" + t.position[2];
	}

	{
		Entity *e = ents->get(11);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(engineControlTime() * 1e-5), Degs());
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
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("texts");

		// entities
		EntityManager *ents = engineEntities();
		{ // floor
			Entity *e = ents->create(1);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/skeletons/floor/floor.obj");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, -5, 0);
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			
			//s.resolution = 2048;
			//s.worldSize = vec3(12, 12, 10);
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
			c.effects = CameraEffectsFlags::Default;
		}
		{ // text hello
			Entity *e = ents->create(11);
			TextComponent &r = e->value<TextComponent>();
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("short/hello");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, -10);
			t.scale = 3;
		}
		{ // text long a
			Entity *e = ents->createAnonymous();
			TextComponent &r = e->value<TextComponent>();
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("long/a");
			r.color = Vec3(1, 0, 0);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(-10, 0, 0);
			t.orientation = Quat(Degs(), Degs(90), Degs());
		}
		{ // text long b
			Entity *e = ents->createAnonymous();
			TextComponent &r = e->value<TextComponent>();
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("long/b");
			r.color = Vec3(0, 0, 1);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(10, 0, 0);
			t.orientation = Quat(Degs(), Degs(-90), Degs());
		}
		{ // text params
			Entity *e = ents->create(10);
			TextComponent &r = e->value<TextComponent>();
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("params/a");
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 0, 10);
			t.orientation = Quat(Degs(), Degs(180), Degs());
		}
		{ // fonts
			for (uint32 i = 0; i < fontsCount; i++)
			{
				Entity *e = ents->createAnonymous();
				TextComponent &r = e->value<TextComponent>();
				r.value = labelTexts[i];
				r.font = HashString(fontNames[i]);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(0, -3, 2.0 * i - fontsCount);
				t.orientation = Quat(Degs(-90), Degs(), Degs());
			}
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(3));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->add(assetsName);
		engineStart();
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
