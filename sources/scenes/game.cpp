#include <vector>

#include <cage-core/assetManager.h>
#include <cage-core/color.h>
#include <cage-core/entities.h>
#include <cage-core/files.h>
#include <cage-core/geometry.h>
#include <cage-core/hashString.h>
#include <cage-core/string.h>
#include <cage-core/timer.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/sceneShadowmapFitting.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;

namespace
{
	Holder<FpsCamera> cameraCtrl;
	Holder<StatisticsGui> statistics;

	uint32 sceneIndexCurrent;
	uint32 sceneIndexLoaded;
	uint32 sceneHashCurrent;

	constexpr uint32 directionalLightsCount = 1;
	Entity *directionalLights[directionalLightsCount];
	constexpr uint32 pointLightsCount = 50;
	Entity *pointLights[pointLightsCount];

	Holder<PointerRange<String>> maps;
}

void loadMapsList()
{
	const String root = pathSearchTowardsRoot("scenes-maps", PathTypeFlags::Directory | PathTypeFlags::Archive);
	maps = pathListDirectory(root);
	CAGE_LOG(SeverityEnum::Info, "scenes", Stringizer() + "found " + maps.size() + " maps");
}

void sceneReload()
{
	CAGE_LOG(SeverityEnum::Info, "scenes", Stringizer() + "loading scene index: " + sceneIndexCurrent);
	const String scenePath = maps[sceneIndexCurrent];
	CAGE_LOG(SeverityEnum::Info, "scenes", Stringizer() + "loading scene description from file: '" + scenePath + "'");
	engineWindow()->title(String() + "scene: " + pathExtractFilename(scenePath));
	engineEntities()->destroy();

	// load new entities
	try
	{
		Holder<File> f = readFile(scenePath);
		String name;
		f->readLine(name);
		if (sceneHashCurrent)
			engineAssets()->remove(sceneHashCurrent);
		sceneHashCurrent = HashString(name.c_str());
		engineAssets()->add(sceneHashCurrent);

		while (f->readLine(name))
		{
			if (name.empty())
				continue;
			Entity *e = engineEntities()->createAnonymous();
			RenderComponent &rs = e->value<RenderComponent>();
			rs.object = HashString(name.c_str());
			if (!rs.object)
			{
				CAGE_LOG_THROW(Stringizer() + "object: '" + name + "'");
				CAGE_THROW_ERROR(Exception, "object has invalid hash");
			}
			TransformComponent &ts = e->value<TransformComponent>();
			String posLine;
			f->readLine(posLine);
			ts.position = Vec3::parse(posLine);
			String rotLine;
			f->readLine(rotLine);
			ts.orientation = Quat(Rads(), Rads(toFloat(rotLine)), Rads());
		}
		CAGE_LOG(SeverityEnum::Info, "scenes", Stringizer() + "scene contains " + (engineEntities()->group()->count()) + " entities");
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "scenes", "failed to load a scene description");
	}

	{ // camera
		Entity *cam = engineEntities()->create(1);
		cam->value<TransformComponent>().position = Vec3(0, 10, 30);
		CameraComponent &c = cam->value<CameraComponent>();
		c.ambientColor = Vec3(1);
		c.ambientIntensity = 0.02;
		c.near = 0.1;
		c.far = 200;
		cam->value<ScreenSpaceEffectsComponent>().effects |= ScreenSpaceEffectsFlags::EyeAdaptation;
		cameraCtrl->setEntity(cam);
	}

	{ // skybox
		Entity *sky = engineEntities()->create(3);
		sky->value<TransformComponent>();
		sky->value<RenderComponent>().object = HashString("scenes/common/skybox.obj");
		sky->value<TextureAnimationComponent>();
	}

	// directional lights
	for (int i = 0; i < directionalLightsCount; i++)
	{
		directionalLights[i] = engineEntities()->createAnonymous();
		TransformComponent &ts = directionalLights[i]->value<TransformComponent>();
		ts.orientation = Quat(Degs(randomChance() * -20 - 30), Degs(i * 360.0f / (float)directionalLightsCount + randomChance() * 180 / (float)directionalLightsCount), Degs());
		LightComponent &ls = directionalLights[i]->value<LightComponent>();
		ls.color = Vec3(1);
		ls.intensity = 2.5;
		ls.lightType = LightTypeEnum::Directional;
		ShadowmapComponent &ss = directionalLights[i]->value<ShadowmapComponent>();
		ss.resolution = 2048;
	}

	// point lights
	for (int i = 0; i < pointLightsCount; i++)
	{
		pointLights[i] = engineEntities()->createAnonymous();
		TransformComponent &ts = pointLights[i]->value<TransformComponent>();
		ts.position = (Vec3(randomChance(), randomChance(), randomChance()) * 2 - 1) * 15;
		LightComponent &ls = pointLights[i]->value<LightComponent>();
		ls.color = colorHsvToRgb(Vec3(randomChance(), 1, 1));
		ls.lightType = LightTypeEnum::Point;
		RenderComponent &r = pointLights[i]->value<RenderComponent>();
		r.color = ls.color;
		r.object = HashString("scenes/common/lightbulb.obj");
	}
}

void windowClose(InputWindow)
{
	engineStop();
}

void keyPress(InputKey in)
{
	switch (in.key)
	{
		case 256: // esc
			engineStop();
			break;
		case 82: // R
			if (sceneIndexCurrent == 0)
				sceneIndexCurrent = numeric_cast<uint32>(maps.size()) - 1;
			else
				sceneIndexCurrent--;
			break;
		case 70: // F
			sceneIndexCurrent++;
			if (sceneIndexCurrent == maps.size())
				sceneIndexCurrent = 0;
			break;
	}
}

void update()
{
#if 0
	{ // automatic reloading -> used for engine testing
		static uint64 last = 0;
		uint64 now = applicationTime();
		if (now > last + (sin(Rads(now) * 2e-7) * 2.5 + 3) * 1000000)
		{
			sceneIndexCurrent++;
			if (sceneIndexCurrent == maps.size())
				sceneIndexCurrent = 0;
			last = now;
		}
	}
#endif

	if (sceneIndexCurrent != sceneIndexLoaded)
	{
		sceneReload();
		sceneIndexLoaded = sceneIndexCurrent;
	}

	// update shadowmaps
	for (int i = 0; i < directionalLightsCount; i++)
	{
		ShadowmapFittingConfig cfg;
		cfg.assets = engineAssets();
		cfg.light = directionalLights[i];
		cfg.camera = engineEntities()->get(1);
		shadowmapFitting(cfg);
	}
}

void actionPrev()
{
	if (sceneIndexCurrent == 0)
		sceneIndexCurrent = numeric_cast<uint32>(maps.size()) - 1;
	else
		sceneIndexCurrent--;
}

void actionNext()
{
	sceneIndexCurrent++;
	if (sceneIndexCurrent == maps.size())
		sceneIndexCurrent = 0;
}

void cameraInitialize()
{
	cameraCtrl = newFpsCamera();
	cameraCtrl->movementSpeed = 0.5;
	cameraCtrl->mouseButton = MouseButtonsFlags::Left;
	statistics = newStatisticsGui();
}

void updateInitialize()
{
	loadMapsList();
	sceneReload();
	engineAssets()->add(HashString("scenes/common/common.pack"));
	Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
	{
		auto _ = g->alignment(Vec2(0, 1));
		g->button().text("< prev").bind<&actionPrev>();
	}
	{
		auto _ = g->alignment(Vec2(1, 1));
		g->button().text("next >").bind<&actionNext>();
	}
}

void updateFinalize()
{
	statistics.clear();
	cameraCtrl.clear();
	engineAssets()->remove(HashString("scenes/common/common.pack"));
	if (sceneHashCurrent)
		engineAssets()->remove(sceneHashCurrent);
	sceneHashCurrent = 0;
}
