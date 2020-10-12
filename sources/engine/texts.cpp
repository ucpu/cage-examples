#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>
#include <cage-core/noiseFunction.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

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
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.position = vec3(noise1->evaluate(engineControlTime()) * 2, noise2->evaluate(engineControlTime()) * 2, 10);
		CAGE_COMPONENT_ENGINE(Text, r, e);
		r.value = stringizer() + t.position[0] + "|" + t.position[1] + "|" + t.position[2];
	}

	{
		Entity *e = ents->get(11);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(), degs(engineControlTime() * 1e-5), degs());
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
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/skeletons/floor/floor.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, -5, 0);
		}
		{ // sun
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Directional;
			l.color = vec3(1);
			l.intensity = 3;
			//CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			//s.resolution = 2048;
			//s.worldSize = vec3(12, 12, 10);
		}
		{ // camera
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.02;
			c.near = 0.1;
			c.far = 100;
			c.effects = CameraEffectsFlags::Default;
		}
		{ // text hello
			Entity *e = ents->create(11);
			CAGE_COMPONENT_ENGINE(Text, r, e);
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("short/hello");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 0, -10);
			t.scale = 3;
		}
		{ // text long a
			Entity *e = ents->createAnonymous();
			CAGE_COMPONENT_ENGINE(Text, r, e);
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("long/a");
			r.color = vec3(1, 0, 0);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(-10, 0, 0);
			t.orientation = quat(degs(), degs(90), degs());
		}
		{ // text long b
			Entity *e = ents->createAnonymous();
			CAGE_COMPONENT_ENGINE(Text, r, e);
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("long/b");
			r.color = vec3(0, 0, 1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(10, 0, 0);
			t.orientation = quat(degs(), degs(-90), degs());
		}
		{ // text params
			Entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(Text, r, e);
			r.assetName = HashString("cage-tests/texts/texts.textpack");
			r.textName = HashString("params/a");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 0, 10);
			t.orientation = quat(degs(), degs(180), degs());
		}
		{ // fonts
			for (uint32 i = 0; i < fontsCount; i++)
			{
				Entity *e = ents->createAnonymous();
				CAGE_COMPONENT_ENGINE(Text, r, e);
				r.value = labelTexts[i];
				r.font = HashString(fontNames[i]);
				CAGE_COMPONENT_ENGINE(Transform, t, e);
				t.position = vec3(0, -3, 2.0 * i - fontsCount);
				t.orientation = quat(degs(-90), degs(), degs());
			}
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(3));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

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
