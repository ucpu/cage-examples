#include <vector>

#include <cage-core/assetsManager.h>
#include <cage-core/color.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/string.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/formats/formats.pack");

void label(const String &name, const Vec3 &position)
{
	Entity *e = engineEntities()->createAnonymous();
	TextComponent &txt = e->value<TextComponent>();
	txt.value = name;
	TransformComponent &t = e->value<TransformComponent>();
	t.position = position;
	t.scale = 0.2;
	t.orientation = Quat(Degs(), Degs(180), Degs());
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
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("formats");

		// entities
		EntityManager *ents = engineEntities();

		{ // monkeys
			uint32 i = 0;
			constexpr const char *objects[] = {
				"cage-tests/formats/formats_fbx_chin.object",
				"cage-tests/formats/formats_fbx_ears.object",
				"cage-tests/formats/formats_glb_chin.object",
				"cage-tests/formats/formats_glb_ears.object",
				"cage-tests/formats/formats_obj.object",
			};
			for (const char *object : objects)
			{
				Entity *e = ents->create(30 + i);
				e->value<RenderComponent>().object = HashString(object);
				TransformComponent &t = e->value<TransformComponent>();
				t.position = Vec3(i * 3 - 6.f, 1, 0);
				t.scale = 0.6;
				label(remove(String(object), 0, 27), t.position + Vec3(0, 1, 0));
				i++;
			}
		}

		{ // floor
			Entity *e = ents->create(100);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/skeletons/floor/floor.obj");
			TransformComponent &t = e->value<TransformComponent>();
			(void)t;
		}

		{ // sun
			Entity *e = ents->create(101);
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-50), Degs(-42 + 180), Degs());
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 2;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.directionalWorldSize = 12;
		}

		{ // camera
			Entity *e = ents->create(102);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(0, 5, -10);
			t.orientation = Quat(Degs(-15), Degs(180), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
			c.near = 0.1;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>();
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(102));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
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
