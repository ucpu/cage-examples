#include <vector>

#include <cage-core/assetsManager.h>
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

using namespace cage;

namespace
{
	Holder<FpsCamera> cameraCtrl;

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
			engineAssets()->unload(sceneHashCurrent);
		sceneHashCurrent = HashString(name.c_str());
		engineAssets()->load(sceneHashCurrent);

		while (f->readLine(name))
		{
			if (name.empty())
				continue;
			Entity *e = engineEntities()->createAnonymous();
			ModelComponent &rs = e->value<ModelComponent>();
			rs.model = HashString(name.c_str());
			if (!rs.model)
			{
				CAGE_LOG_THROW(Stringizer() + "object: " + name);
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
		CAGE_LOG(SeverityEnum::Info, "scenes", Stringizer() + "scene contains " + (engineEntities()->count()) + " entities");
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
		sky->value<ModelComponent>().model = HashString("scenes/common/skybox.obj");
	}

	// directional lights
	for (int i = 0; i < directionalLightsCount; i++)
	{
		Entity *e = directionalLights[i] = engineEntities()->createUnique();
		e->value<TransformComponent>().orientation = Quat(Degs(randomChance() * -20 - 30), Degs(i * 360.0f / (float)directionalLightsCount + randomChance() * 180 / (float)directionalLightsCount), Degs());
		e->value<LightComponent>().lightType = LightTypeEnum::Directional;
		e->value<ColorComponent>().intensity = 2.5;
		e->value<ShadowmapComponent>().resolution = 2048;
	}

	// point lights
	for (int i = 0; i < pointLightsCount; i++)
	{
		Entity *e = pointLights[i] = engineEntities()->createAnonymous();
		e->value<TransformComponent>().position = (randomChance3() * 2 - 1) * 15;
		e->value<ColorComponent>().color = colorHsvToRgb(Vec3(randomChance(), 1, 1));
		e->value<LightComponent>().minDistance = 0.1;
		e->value<LightComponent>().maxDistance = 10;
		e->value<ModelComponent>().model = HashString("scenes/common/lightbulb.obj");
	}
}

void keyPress(input::KeyPress in)
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
#if 1
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

void actionPrev(input::GuiValue)
{
	if (sceneIndexCurrent == 0)
		sceneIndexCurrent = numeric_cast<uint32>(maps.size()) - 1;
	else
		sceneIndexCurrent--;
}

void actionNext(input::GuiValue)
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
}

void updateInitialize()
{
	loadMapsList();
	sceneReload();
	engineAssets()->load(HashString("scenes/common/common.pack"));
	Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
	{
		auto _ = g->alignment(Vec2(0, 1));
		g->button().text("< prev").event(inputFilter(actionPrev));
	}
	{
		auto _ = g->alignment(Vec2(1, 1));
		g->button().text("next >").event(inputFilter(actionNext));
	}
}

void updateFinalize()
{
	cameraCtrl.clear();
	engineAssets()->unload(HashString("scenes/common/common.pack"));
	if (sceneHashCurrent)
		engineAssets()->unload(sceneHashCurrent);
	sceneHashCurrent = 0;
}
