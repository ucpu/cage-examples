#include <vector>

#include <cage-core/assetsManager.h>
#include <cage-core/color.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/bottle/bottle.pack");

Vec3 getGuiColor(uint32 id)
{
	Entity *e = engineGuiEntities()->get(id);
	GuiColorPickerComponent &c = e->value<GuiColorPickerComponent>();
	return c.color;
}

Vec3 getGuiOrientation(uint32 id)
{
	Vec3 result;
	EntityManager *ents = engineGuiEntities();
	for (uint32 i = 0; i < 2; i++)
	{
		Entity *e = ents->get(id + i);
		GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
		result[i] = c.value;
	}
	return result;
}

void update()
{
	EntityManager *ents = engineEntities();

	{ // update ambient light
		Entity *e = ents->get(10);
		e->value<CameraComponent>().ambientColor = getGuiColor(27); // ambient light
		e->value<ColorComponent>().color = getGuiColor(28); // headlight
	}

	for (uint32 i = 0; i < 3; i++)
	{ // update lights
		Entity *e = ents->get(5 + i);
		e->value<ColorComponent>().color = getGuiColor(37 + i * 10);
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
}

void initializeGuiColors(uint32 parentId, uint32 id, const Vec3 &hsv)
{
	Entity *e = engineGuiEntities()->create(id);
	GuiParentComponent &p = e->value<GuiParentComponent>();
	p.parent = parentId;
	p.order = id;
	GuiColorPickerComponent &c = e->value<GuiColorPickerComponent>();
	c.color = colorHsvToRgb(hsv);
	c.collapsible = true;
}

void initializeGui()
{
	EntityManager *ents = engineGuiEntities();
	Entity *layout = ents->create(1);
	{ // layout
		layout->value<GuiLayoutScrollbarsComponent>();
		layout->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0);
		layout->value<GuiLayoutLineComponent>().vertical = true;
	}

	{ // ambient
		Entity *panel = ents->create(20);
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->id();
			p.order = 1;
			panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = "Ambient & Headlight";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		initializeGuiColors(panel->id(), 27, Vec3(0, 0, 0.1));
		initializeGuiColors(panel->id(), 28, Vec3(0, 0, 0.2));
	}

	static constexpr const Vec3 colors[3] = { Vec3(0.11, 0.95, 0.8), Vec3(0.44, 0.95, 0.8), Vec3(0.77, 0.95, 0.8) };
	static constexpr const Real pitches[3] = { 0.25 * 90, 0.3 * 90, 0.55 * 90 };
	static constexpr const Real yaws[3] = { 0.1 * 360, 0.2 * 360, 0.6 * 360 };
	for (uint32 i = 0; i < 3; i++)
	{ // spot lights
		Entity *panel = ents->create(30 + i * 10);
		{
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = layout->id();
			p.order = 5 + i;
			panel->value<GuiSpoilerComponent>();
			GuiTextComponent &t = panel->value<GuiTextComponent>();
			t.value = Stringizer() + "Spot light [" + i + "]:";
			GuiLayoutLineComponent &l = panel->value<GuiLayoutLineComponent>();
			l.vertical = true;
		}
		{
			Entity *e = ents->create(33 + i * 10);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->id();
			p.order = 2;
			GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
			c.value = pitches[i].value;
			c.min = 0;
			c.max = 90;
		}
		{
			Entity *e = ents->create(34 + i * 10);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = panel->id();
			p.order = 3;
			GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
			c.value = yaws[i].value;
			c.min = 0;
			c.max = 360;
		}
		initializeGuiColors(panel->id(), 37 + i * 10, colors[i]);
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("lights");
		initializeGui();

		// entities
		EntityManager *ents = engineEntities();
		{ // floor
			Entity *e = ents->create(1);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/other.obj?plane");
			e->value<TransformComponent>();
		}
		{ // bottle
			Entity *e = ents->create(2);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/bottle.obj");
			e->value<TransformComponent>().position += Vec3(1, 0, 0);
		}
		for (uint32 i = 0; i < 3; i++)
		{ // spot lights
			Entity *e = ents->create(5 + i);
			e->value<ModelComponent>().model = HashString("cage-tests/bottle/other.obj?arrow");
			//e->value<ModelComponent>().model = HashString("cage/models/axes.obj");
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Spot;
			l.spotAngle = Degs(40);
			l.spotExponent = 40;
			l.minDistance = 3;
			l.maxDistance = 50;
			e->value<ColorComponent>().intensity = 100;
			e->value<ShadowmapComponent>().resolution = 1024;
			e->value<ShadowmapComponent>().shadowFactor = 0.5;
		}
		{ // camera
			Entity *e = ents->create(10);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 7);
			t.orientation = Quat(Degs(-10), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			//e->value<ScreenSpaceEffectsComponent>();
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.ssaoFactor = 1;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(10));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.3;
		Holder<StatisticsGui> statistics = newStatisticsGui();

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
