#include <cage-core/assetsManager.h>
#include <cage-core/entitiesVisitor.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

constexpr uint32 AssetsName = HashString("cage-tests/camera-effects/effects.pack");

struct DataComponent
{
	uint64 destroyTime = 0;
};

void addEntities(uint32 cnt)
{
	const uint64 time = engineControlTime();
	for (uint32 i = 0; i < cnt; i++)
	{
		Entity *e = engineEntities()->createAnonymous();
		e->value<DataComponent>().destroyTime = time + randomRange(200, 300) * 1'000;
		e->value<RenderComponent>().object = HashString("scenes/mcguire/crytek/sponza.object");
		e->value<RenderComponent>().opacity = randomChance() * 0.4 + 0.3; // prevent depth pre-pass optimization
		e->value<TransformComponent>().position += randomDirection3() * randomRange(0.01, 0.1);
	}
}

void update()
{
	const uint64 time = engineControlTime();
	entitiesVisitor(
		[time](Entity *e, const DataComponent &d)
		{
			if (time > d.destroyTime)
				e->destroy();
		},
		engineEntities(), true);

	const sint32 cnt = numeric_cast<sint32>((sin(Degs(time / 200'000)) + 0.8) * 20);
	if (cnt > 0)
		addEntities(cnt);
}

void keyPress(input::KeyPress in)
{
	if (in.key == 32) // space
		addEntities(100);
}

void generateGui()
{
	Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
	auto _1 = g->alignment(Vec2(0));
	auto _2 = g->spoiler(false).text("Dynamic Resolution");
	auto _3 = g->verticalTable(3);
	{
		g->label().text("Enabled: ");
		g->checkBox(engineDynamicResolution().enabled).event(inputFilter([](input::GuiValue v) { engineDynamicResolution().enabled = v.entity->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked; }));
		g->empty();
	}
	{
		g->label().text("Target FPS: ");
		g->horizontalSliderBar(engineDynamicResolution().targetFps, 1, 90).event(inputFilter([](input::GuiValue v) { engineDynamicResolution().targetFps = v.entity->value<GuiSliderBarComponent>().value.value; }));
		g->label().text("").update([](Entity *e) { e->value<GuiTextComponent>().value = Stringizer() + engineDynamicResolution().targetFps; });
	}
	{
		g->label().text("Minimum Scale: ");
		g->horizontalSliderBar(engineDynamicResolution().minimumScale, 0.1, 1.0).event(inputFilter([](input::GuiValue v) { engineDynamicResolution().minimumScale = v.entity->value<GuiSliderBarComponent>().value; }));
		g->label().text("").update([](Entity *e) { e->value<GuiTextComponent>().value = Stringizer() + engineDynamicResolution().minimumScale; });
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));
		const auto keyPressListener = engineWindow()->events.listen(inputFilter(keyPress));

		// window
		engineWindow()->title("dynamic resolution");
		engineWindow()->setMaximized();
		generateGui();

		// entities
		EntityManager *ents = engineEntities();
		ents->defineComponent(DataComponent());
		{ // camera
			Entity *e = ents->create(1);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(11.5, 1, -1);
			t.orientation = Quat(Degs(10), Degs(110), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.05;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}
		{ // skybox
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>();
			e->value<RenderComponent>().object = HashString("scenes/common/skybox.obj");
			e->value<TextureAnimationComponent>();
		}
		{ // sun
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, 0);
			t.orientation = Quat(Degs(-75), Degs(-120), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 3;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.directionalWorldSize = 30;
		}
		{ // floor
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/common/ground.obj");
			e->value<TransformComponent>().position = Vec3(0, -1.264425, 0);
		}
		{ // sponza
			Entity *e = ents->createAnonymous();
			e->value<RenderComponent>().object = HashString("scenes/mcguire/crytek/sponza.object");
			e->value<TransformComponent>();
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(1));
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
