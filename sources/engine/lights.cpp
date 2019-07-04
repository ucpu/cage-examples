#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/bottle/bottle.pack");

bool windowClose()
{
	engineStop();
	return false;
}

vec3 getGuiColor(uint32 id)
{
	entity *e = gui()->entities()->get(id);
	CAGE_COMPONENT_GUI(colorPicker, c, e);
	return c.color;
}

vec3 getGuiOrientation(uint32 id)
{
	vec3 result;
	entityManager *ents = gui()->entities();
	for (uint32 i = 0; i < 2; i++)
	{
		entity *e = ents->get(id + i);
		CAGE_COMPONENT_GUI(sliderBar, c, e);
		result[i] = c.value;
	}
	return result;
}

bool update()
{
	entityManager *ents = entities();

	{ // update ambient light
		entity *e = ents->get(10);
		CAGE_COMPONENT_ENGINE(camera, c, e);
		c.ambientLight = getGuiColor(27);
	}

	for (uint32 i = 0; i < 3; i++)
	{ // update lights
		entity *e = ents->get(5 + i);
		CAGE_COMPONENT_ENGINE(render, r, e);
		CAGE_COMPONENT_ENGINE(light, l, e);
		r.color = l.color = getGuiColor(37 + i * 10);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		vec3 o = getGuiOrientation(33 + i * 10);
		t.orientation = quat(degs(o[0] - 90), degs(o[1]), degs());
		t.position = vec3(0, 3, 0) + t.orientation * vec3(0, 0, 10);
	}

	{ // rotate the bottle
		entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(transform, t, e);
		t.orientation = quat(degs(), degs(1), degs()) * t.orientation;
	}
	return false;
}

void initializeGuiColors(uint32 parentId, uint32 id, const vec3 &hsv)
{
	entity *e = gui()->entities()->create(id);
	CAGE_COMPONENT_GUI(parent, p, e);
	p.parent = parentId;
	p.order = 5;
	CAGE_COMPONENT_GUI(colorPicker, c, e);
	c.color = convertHsvToRgb(hsv);
	c.collapsible = true;
}

void initializeGui()
{
	entityManager *ents = gui()->entities();
	entity *layout = ents->create(1);
	{ // layout
		CAGE_COMPONENT_GUI(scrollbars, sc, layout);
		CAGE_COMPONENT_GUI(layoutLine, l, layout);
		l.vertical = true;
	}

	{ // ambient
		entity *panel = ents->create(20);
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = "Ambient";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		initializeGuiColors(panel->name(), 27, vec3(0, 0, 0.02));
	}

	vec3 colors[3] = {
		vec3(0.11, 0.95, 0.8),
		vec3(0.44, 0.95, 0.8),
		vec3(0.77, 0.95, 0.8)
	};
	real pitches[3] = { 0.25 * 90, 0.3 * 90, 0.55 * 90 };
	real yaws[3] = { 0.1 * 360, 0.2 * 360, 0.6 * 360 };
	for (uint32 i = 0; i < 3; i++)
	{ // spot lights
		entity *panel = ents->create(30 + i * 10);
		{
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = layout->name();
			p.order = 5 + i;
			CAGE_COMPONENT_GUI(spoiler, c, panel);
			CAGE_COMPONENT_GUI(text, t, panel);
			t.value = string() + "Spot light [" + i + "]:";
			CAGE_COMPONENT_GUI(layoutLine, l, panel);
			l.vertical = true;
		}
		{
			entity *e = ents->create(33 + i * 10);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(sliderBar, c, e);
			c.value = pitches[i].value;
			c.min = 0;
			c.max = 90;
		}
		{
			entity *e = ents->create(34 + i * 10);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = panel->name();
			p.order = 3;
			CAGE_COMPONENT_GUI(sliderBar, c, e);
			c.value = yaws[i].value;
			c.min = 0;
			c.max = 360;
		}
		initializeGuiColors(panel->name(), 37 + i * 10, colors[i]);
	}
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
		window()->title("lights");
		initializeGui();

		// entities
		entityManager *ents = entities();
		{ // floor
			entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/bottle/other.obj?plane");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.scale = 5;
		}
		{ // bottle
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/bottle/bottle.obj");
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position += vec3(1, 0, 0);
		}
		for (uint32 i = 0; i < 3; i++)
		{ // spot lights
			entity *e = ents->create(5 + i);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/bottle/other.obj?arrow");
			CAGE_COMPONENT_ENGINE(light, l, e);
			l.lightType = lightTypeEnum::Spot;
			l.spotAngle = degs(40);
			l.spotExponent = 40;
			l.attenuation = vec3(1, 0, 0.005);
			CAGE_COMPONENT_ENGINE(shadowmap, s, e);
			s.resolution = 512;
			s.worldSize = vec3(3, 50, 0);
		}
		{ // camera
			entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 5, 7);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.near = 0.1;
			c.far = 1000;
			c.effects = cameraEffectsFlags::CombinedPass;
		}

		holder<cameraController> cameraController = newCameraController(ents->get(10));
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.3;
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
