#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assets.h>
#include <cage-core/noise.h>
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

noiseCreateConfig noiseInit(uint32 seed)
{
	noiseCreateConfig cfg;
	cfg.seed = seed;
	cfg.type = noiseTypeEnum::Cellular;
	cfg.frequency = 1e-7;
	return cfg;
}

bool update()
{
	entityManagerClass *ents = entities();

	{
		static holder<noiseClass> noise1 = newNoise(noiseInit(42));
		static holder<noiseClass> noise2 = newNoise(noiseInit(13));
		entityClass *e = ents->get(10);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.position = vec3(noise1->evaluate(currentControlTime()) * 2, noise2->evaluate(currentControlTime()) * 2, 10);
		ENGINE_GET_COMPONENT(renderText, r, e);
		r.value = string() + t.position[0] + "|" + t.position[1] + "|" + t.position[2];
	}

	{
		entityClass *e = ents->get(11);
		ENGINE_GET_COMPONENT(transform, t, e);
		t.orientation = quat(degs(), degs(currentControlTime() * 1e-5), degs());
	}

	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("texts");

		// entities
		entityManagerClass *ents = entities();
		{ // floor
			entityClass *e = ents->create(1);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/skeletons/floor/floor.obj");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, -5, 0);
		}
		{ // sun
			entityClass *e = ents->create(2);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-50), degs(-42 + 180), degs());
			ENGINE_GET_COMPONENT(light, l, e);
			l.lightType = lightTypeEnum::Directional;
			l.color = vec3(1, 1, 1) * 3;
			//ENGINE_GET_COMPONENT(shadowmap, s, e);
			//s.resolution = 2048;
			//s.worldSize = vec3(12, 12, 10);
		}
		{ // camera
			entityClass *e = ents->create(3);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.orientation = quat(degs(-10), degs(), degs());
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.02;
			c.near = 0.1;
			c.far = 100;
			c.effects = cameraEffectsFlags::CombinedPass;
		}
		{ // text hello
			entityClass *e = ents->create(11);
			ENGINE_GET_COMPONENT(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("short/hello");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 0, -10);
			t.scale = 3;
		}
		{ // text long a
			entityClass *e = ents->createAnonymous();
			ENGINE_GET_COMPONENT(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("long/a");
			r.color = vec3(1, 0, 0);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(-10, 0, 0);
			t.orientation = quat(degs(), degs(90), degs());
		}
		{ // text long b
			entityClass *e = ents->createAnonymous();
			ENGINE_GET_COMPONENT(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("long/b");
			r.color = vec3(0, 0, 1);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(10, 0, 0);
			t.orientation = quat(degs(), degs(-90), degs());
		}
		{ // text params
			entityClass *e = ents->create(10);
			ENGINE_GET_COMPONENT(renderText, r, e);
			r.assetName = hashString("cage-tests/texts/texts.textpack");
			r.textName = hashString("params/a");
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 0, 10);
			t.orientation = quat(degs(), degs(180), degs());
		}
		{ // fonts
			for (uint32 i = 0; i < fontsCount; i++)
			{
				entityClass *e = ents->createAnonymous();
				ENGINE_GET_COMPONENT(renderText, r, e);
				r.value = labelTexts[i];
				r.font = hashString(fontNames[i]);
				ENGINE_GET_COMPONENT(transform, t, e);
				t.position = vec3(0, -3, 2.0 * i - fontsCount);
				t.orientation = quat(degs(-90), degs(), degs());
			}
		}

		holder<cameraControllerClass> cameraController = newCameraController(ents->get(3));
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.3;
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

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
