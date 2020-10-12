#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/bottle/bottle.pack");

bool windowClose()
{
	engineStop();
	return false;
}

vec3 getGuiColor(uint32 id)
{
	Entity *e = engineGui()->entities()->get(id);
	CAGE_COMPONENT_GUI(ColorPicker, c, e);
	return c.color;
}

vec3 getGuiOrientation(uint32 id)
{
	vec3 result;
	EntityManager *ents = engineGui()->entities();
	for (uint32 i = 0; i < 2; i++)
	{
		Entity *e = ents->get(id + i);
		CAGE_COMPONENT_GUI(SliderBar, c, e);
		result[i] = c.value;
	}
	return result;
}

bool update()
{
	EntityManager *ents = engineEntities();

	{ // update ambient light
		Entity *e = ents->get(10);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientColor = getGuiColor(27);
		c.ambientDirectionalColor = getGuiColor(28);
	}

	for (uint32 i = 0; i < 3; i++)
	{ // update lights
		Entity *e = ents->get(5 + i);
		CAGE_COMPONENT_ENGINE(Render, r, e);
		CAGE_COMPONENT_ENGINE(Light, l, e);
		r.color = l.color = getGuiColor(37 + i * 10);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		vec3 o = getGuiOrientation(33 + i * 10);
		t.orientation = quat(degs(o[0] - 90), degs(o[1]), degs());
		t.position = vec3(0, 3, 0) + t.orientation * vec3(0, 0, 10);
	}

	{ // rotate the bottle
		Entity *e = ents->get(2);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = quat(degs(), degs(1), degs()) * t.orientation;
	}
	return false;
}

void initializeGuiColors(uint32 parentId, uint32 id, const vec3 &hsv)
{
	Entity *e = engineGui()->entities()->create(id);
	CAGE_COMPONENT_GUI(Parent, p, e);
	p.parent = parentId;
	p.order = id;
	CAGE_COMPONENT_GUI(ColorPicker, c, e);
	c.color = colorHsvToRgb(hsv);
	c.collapsible = true;
}

void initializeGui()
{
	EntityManager *ents = engineGui()->entities();
	Entity *layout = ents->create(1);
	{ // layout
		CAGE_COMPONENT_GUI(Scrollbars, sc, layout);
		CAGE_COMPONENT_GUI(LayoutLine, l, layout);
		l.vertical = true;
	}

	{ // ambient
		Entity *panel = ents->create(20);
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 1;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = "Ambient";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		initializeGuiColors(panel->name(), 27, vec3(0, 0, 0.02));
		initializeGuiColors(panel->name(), 28, vec3(0, 0, 0.02));
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
		Entity *panel = ents->create(30 + i * 10);
		{
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = layout->name();
			p.order = 5 + i;
			CAGE_COMPONENT_GUI(Spoiler, c, panel);
			CAGE_COMPONENT_GUI(Text, t, panel);
			t.value = stringizer() + "Spot light [" + i + "]:";
			CAGE_COMPONENT_GUI(LayoutLine, l, panel);
			l.vertical = true;
		}
		{
			Entity *e = ents->create(33 + i * 10);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(SliderBar, c, e);
			c.value = pitches[i].value;
			c.min = 0;
			c.max = 90;
		}
		{
			Entity *e = ents->create(34 + i * 10);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = panel->name();
			p.order = 3;
			CAGE_COMPONENT_GUI(SliderBar, c, e);
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
		engineWindow()->title("lights");
		initializeGui();

		// entities
		EntityManager *ents = engineEntities();
		{ // floor
			Entity *e = ents->create(1);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/bottle/other.obj?plane");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.scale = 5;
		}
		{ // bottle
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/bottle/bottle.obj");
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position += vec3(1, 0, 0);
		}
		for (uint32 i = 0; i < 3; i++)
		{ // spot lights
			Entity *e = ents->create(5 + i);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/bottle/other.obj?arrow");
			CAGE_COMPONENT_ENGINE(Light, l, e);
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = degs(40);
			l.spotExponent = 40;
			l.attenuation = vec3(1, 0, 0.005);
			CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
			s.resolution = 512;
			s.worldSize = vec3(3, 50, 0);
		}
		{ // camera
			Entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 5, 7);
			t.orientation = quat(degs(-10), degs(), degs());
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.near = 0.1;
			c.far = 1000;
			c.effects = CameraEffectsFlags::Default;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(10));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
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
