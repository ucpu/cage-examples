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
#include <cage-engine/engineStatistics.h>
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

Vec3 getGuiColor(uint32 id)
{
	Entity *e = engineGui()->entities()->get(id);
	GuiColorPickerComponent &c = e->value<GuiColorPickerComponent>();
	return c.color;
}

Vec3 getGuiOrientation(uint32 id)
{
	Vec3 result;
	EntityManager *ents = engineGui()->entities();
	for (uint32 i = 0; i < 2; i++)
	{
		Entity *e = ents->get(id + i);
		GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
		result[i] = c.value;
	}
	return result;
}

bool update()
{
	EntityManager *ents = engineEntities();

	{ // update ambient light
		Entity *e = ents->get(10);
		CameraComponent &c = e->value<CameraComponent>();
		c.ambientColor = getGuiColor(27);
		c.ambientDirectionalColor = getGuiColor(28);
	}

	for (uint32 i = 0; i < 3; i++)
	{ // update lights
		Entity *e = ents->get(5 + i);
		RenderComponent &r = e->value<RenderComponent>();
		LightComponent &l = e->value<LightComponent>();
		r.color = l.color = getGuiColor(37 + i * 10);
		TransformComponent &t = e->value<TransformComponent>();
		Vec3 o = getGuiOrientation(33 + i * 10);
		t.orientation = Quat(Degs(o[0] - 90), Degs(o[1]), Degs());
		t.position = Vec3(0, 3, 0) + t.orientation * Vec3(0, 0, 10);
	}

	{ // rotate the bottle
		Entity *e = ents->get(2);
		TransformComponent &t = e->value<TransformComponent>();
		t.orientation = Quat(Degs(), Degs(1), Degs()) * t.orientation;
	}
	return false;
}

void initializeGuiColors(uint32 parentId, uint32 id, const Vec3 &hsv)
{
	Entity *e = engineGui()->entities()->create(id);
	GuiParentComponent &p = e->value<GuiParentComponent>();
	p.parent = parentId;
	p.order = id;
	GuiColorPickerComponent &c = e->value<GuiColorPickerComponent>();
	c.color = colorHsvToRgb(hsv);
	c.collapsible = true;
}

void initializeGui()
{
	EntityManager *ents = engineGui()->entities();
	Entity *layout = ents->create(1);
	{ // layout
		GuiScrollbarsComponent &sc = layout->value<GuiScrollbarsComponent>();
		GuiLayoutLineComponent &l = layout->value<GuiLayoutLineComponent>();
		l.vertical = true;
	}

	{ // ambient
		Entity *panel = ents->create(20);
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = 1;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Ambient";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		initializeGuiColors(panel->name(), 27, Vec3(0, 0, 0.02));
		initializeGuiColors(panel->name(), 28, Vec3(0, 0, 0.02));
	}

	Vec3 colors[3] = {
		Vec3(0.11, 0.95, 0.8),
		Vec3(0.44, 0.95, 0.8),
		Vec3(0.77, 0.95, 0.8)
	};
	Real pitches[3] = { 0.25 * 90, 0.3 * 90, 0.55 * 90 };
	Real yaws[3] = { 0.1 * 360, 0.2 * 360, 0.6 * 360 };
	for (uint32 i = 0; i < 3; i++)
	{ // spot lights
		Entity *panel = ents->create(30 + i * 10);
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->name();
			p.order = 5 + i;
			GuiSpoilerComponent &c = panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = Stringizer() + "Spot light [" + i + "]:";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{
			Entity *e = ents->create(33 + i * 10);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 2;
			GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
			c.value = pitches[i].value;
			c.min = 0;
			c.max = 90;
		}
		{
			Entity *e = ents->create(34 + i * 10);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->name();
			p.order = 3;
			GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
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
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/bottle/other.obj?plane");
			TransformComponent &t = e->value<TransformComponent>();
			t.scale = 5;
		}
		{ // bottle
			Entity *e = ents->create(2);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/bottle/bottle.obj");
			TransformComponent &t = e->value<TransformComponent>();
			t.position += Vec3(1, 0, 0);
		}
		for (uint32 i = 0; i < 3; i++)
		{ // spot lights
			Entity *e = ents->create(5 + i);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/bottle/other.obj?arrow");
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = Degs(40);
			l.spotExponent = 40;
			l.attenuation = Vec3(1, 0, 0.005);
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 512;
			s.worldSize = Vec3(3, 50, 0);
		}
		{ // camera
			Entity *e = ents->create(10);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 7);
			t.orientation = Quat(Degs(-10), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.effects = CameraEffectsFlags::Default;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(10));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<EngineStatistics> statistics = newEngineStatistics();

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
