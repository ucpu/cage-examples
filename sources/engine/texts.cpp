#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>
#include <cage-core/noiseFunction.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/texts/texts.pack");

const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/Ubuntu-R.ttf",
};
const string labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu",
};

const uint32 fontsCount = sizeof(fontNames) / sizeof(fontNames[0]);
CAGE_ASSERT_COMPILE(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), arrays_must_have_same_number_of_elements);

bool windowClose()
{
	engineStop();
	return false;
}

noiseFunctionCreateConfig noiseInit(uint32 seed)
{
	noiseFunctionCreateConfig cfg;
	cfg.seed = seed;
	cfg.type = noiseTypeEnum::Cellular;
	cfg.frequency = 1e-7;
	return cfg;
}

bool update()
{
	entityManager *ents = entities();

	{
		static holder<noiseFunction> noise1 = newNoiseFunction(noiseInit(42));
		static holder<noiseFunction> noise2 = newNoiseFunction(noiseInit(13));
		entity *e = ents->get(10);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.position = vec3(noise1->evaluate(currentControlTime()) * 2, noise2->evaluate(currentControlTime()) * 2, 10);
		CAGE_COMPONENT_ENGINE(renderText, r, e);
		r.value = string() + t.position[0] + "|" + t.position[1] + "|" + t.position[2];
	}

	{
		entity *e = ents->get(11);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.orientation = quat(degs(), degs(currentControlTime() * 1e-5), degs());
	}

	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("texts");

		// entities
		entityManager *ents = entities();
		{ // floor
			entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/skeletons/floor/floor.obj");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, -5, 0);
		}
		{ // sun
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(1, 1, 1) * 3;
			//CAGE_COMPONENT_ENGINE(shadowmap, s, e);
			//s.resolution = 2048;
			//s.worldSize = vec3(12, 12, 10);
		}
		{ // camera
			entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.02;
			c.near = 0.1;
			c.far = 100;
			c.effects = cameraEffectsFlags::CombinedPass;
		}
		{ // text hello
			entity *e = ents->create(11);
			CAGE_COMPONENT_ENGINE(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("short/hello");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 0, -10);
			t.scale = 3;
		}
		{ // text long a
			entity *e = ents->createAnonymous();
			CAGE_COMPONENT_ENGINE(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("long/a");
			r.color = vec3(1, 0, 0);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(-10, 0, 0);
			t.orientation = quat(degs(), degs(90), degs());
		}
		{ // text long b
			entity *e = ents->createAnonymous();
			CAGE_COMPONENT_ENGINE(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("long/b");
			r.color = vec3(0, 0, 1);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(10, 0, 0);
			t.orientation = quat(degs(), degs(-90), degs());
		}
		{ // text params
			entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("params/a");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 0, 10);
			t.orientation = quat(degs(), degs(180), degs());
		}
		{ // fonts
			for (uint32 i = 0; i < fontsCount; i++)
			{
				entity *e = ents->createAnonymous();
				CAGE_COMPONENT_ENGINE(renderText, r, e);
				r.value = labelTexts[i];
				r.font = hashString(fontNames[i]);
				CAGE_COMPONENT_ENGINE(transform, t, e);
				t.position = vec3(0, -3, 2.0 * i - fontsCount);
				t.orientation = quat(degs(-90), degs(), degs());
			}
		}

		holder<cameraController> cameraCtrl = newCameraController(ents->get(3));
		cameraCtrl->mouseButton = mouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
		holder<engineProfiling> engineProfiling = newEngineProfiling();

		assets()->add(assetsName);
		engineStart();
		assets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
