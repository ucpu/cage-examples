#include "game.h"

#include <cage-core/geometry.h>
#include <cage-core/string.h>
#include <cage-engine/shadowmapHelpers.h>

namespace
{
	Holder<FpsCamera> cameraCtrl;
	Holder<EngineProfiling> engineProfilingInstance;

	uint32 sceneIndexCurrent;
	uint32 sceneIndexLoaded;
	uint32 sceneHashCurrent;

	static const uint32 directionalLightsCount = 1;
	Entity *directionalLights[directionalLightsCount];
	static const uint32 pointLightsCount = 50;
	Entity *pointLights[pointLightsCount];

	std::vector<string> maps;
}

void loadMapsList()
{
	string root = pathSearchTowardsRoot("scenes-maps", PathTypeFlags::Directory | PathTypeFlags::Archive);
	Holder<DirectoryList> list = newDirectoryList(root);
	while (list->valid())
	{
		maps.push_back(pathJoin(root, list->name()));
		list->next();
	}
	CAGE_LOG(SeverityEnum::Info, "scenes", stringizer() + "found " + maps.size() + " maps");
}

void sceneReload()
{
	CAGE_LOG(SeverityEnum::Info, "scenes", stringizer() + "loading scene index: " + sceneIndexCurrent);
	string scenePath = maps[sceneIndexCurrent];
	CAGE_LOG(SeverityEnum::Info, "scenes", stringizer() + "loading scene description from file: '" + scenePath + "'");
	engineWindow()->title(string() + "scene: " + pathExtractFilename(scenePath));
	engineEntities()->destroy();

	// load new entities
	try
	{
		Holder<File> f = newFile(scenePath, FileMode(true, false));
		string name;
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
			CAGE_COMPONENT_ENGINE(Render, rs, e);
			rs.object = HashString(name.c_str());
			if (!rs.object)
			{
				CAGE_LOG_THROW(stringizer() + "object: '" + name + "'");
				CAGE_THROW_ERROR(Exception, "object has invalid hash");
			}
			CAGE_COMPONENT_ENGINE(Transform, ts, e);
			string posLine;
			f->readLine(posLine);
			ts.position = vec3::parse(posLine);
			string rotLine;
			f->readLine(rotLine);
			ts.orientation = quat(rads(), rads(toFloat(rotLine)), rads());
		}
		CAGE_LOG(SeverityEnum::Info, "scenes", stringizer() + "scene contains " + (engineEntities()->group()->count()) + " entities");
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "scenes", "failed to load a scene description");
	}

	{ // camera
		Entity *cam = engineEntities()->create(1);
		CAGE_COMPONENT_ENGINE(Transform, t, cam);
		t.position = vec3(0, 10, 30);
		CAGE_COMPONENT_ENGINE(Camera, c, cam);
		c.ambientColor = vec3(1);
		c.ambientIntensity = 0.03;
		c.ambientDirectionalColor = vec3(1);
		c.ambientDirectionalIntensity = 0.07;
		c.cameraOrder = 2;
		c.near = 0.1;
		c.far = 200;
		c.clear = CameraClearFlags::None;
		c.effects = CameraEffectsFlags::Default;
		cameraCtrl->setEntity(cam);
	}

	{ // skybox
		Entity *cam = engineEntities()->create(2);
		CAGE_COMPONENT_ENGINE(Transform, tc, cam);
		(void)tc;
		CAGE_COMPONENT_ENGINE(Camera, c, cam);
		c.sceneMask = 2;
		c.cameraOrder = 1;
		c.near = 0.1;
		c.far = 50;
		c.effects = CameraEffectsFlags::MotionBlur; // need to enable motion blur to make sure that velocity buffer is cleared
		Entity *sky = engineEntities()->create(3);
		CAGE_COMPONENT_ENGINE(Transform, ts, sky);
		(void)ts;
		CAGE_COMPONENT_ENGINE(Render, r, sky);
		CAGE_COMPONENT_ENGINE(TextureAnimation, a, sky);
		(void)a;
		r.object = HashString("scenes/common/skybox.obj");
		r.sceneMask = 2;
	}
	
	// directional lights
	for (int i = 0; i < directionalLightsCount; i++)
	{
		directionalLights[i] = engineEntities()->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, ts, directionalLights[i]);
		ts.orientation = quat(degs(randomChance() * -20 - 30), degs(i * 360.0f / (float)directionalLightsCount + randomChance() * 180 / (float)directionalLightsCount), degs());
		CAGE_COMPONENT_ENGINE(Light, ls, directionalLights[i]);
		ls.color = vec3(1);
		ls.intensity = 2.5;
		ls.lightType = LightTypeEnum::Directional;
		CAGE_COMPONENT_ENGINE(Shadowmap, ss, directionalLights[i]);
		ss.resolution = 2048;
	}
	
	// point lights
	for (int i = 0; i < pointLightsCount; i++)
	{
		pointLights[i] = engineEntities()->createAnonymous();
		CAGE_COMPONENT_ENGINE(Transform, ts, pointLights[i]);
		ts.position = (vec3(randomChance(), randomChance(), randomChance()) * 2 - 1) * 15;
		CAGE_COMPONENT_ENGINE(Light, ls, pointLights[i]);
		ls.color = colorHsvToRgb(vec3(randomChance(), 1, 1));
		ls.lightType = LightTypeEnum::Point;
		//CAGE_COMPONENT_ENGINE(Shadowmap, ss, pointLights[i]);
		//ss.worldRadius = vec3(35);
		//ss.resolution = 1024;
		CAGE_COMPONENT_ENGINE(Render, r, pointLights[i]);
		r.color = ls.color;
		r.object = HashString("scenes/common/lightbulb.obj");
	}
}

bool closeButton()
{
	engineStop();
	return false;
}

bool keyPress(uint32 key, ModifiersFlags)
{
	switch (key)
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

	return false;
}

bool update()
{
#if 0
	{ // automatic reloading -> used for engine testing
		static uint64 last = 0;
		uint64 now = applicationTime();
		if (now > last + (sin(rads(now) * 2e-7) * 2.5 + 3) * 1000000)
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

	{ // update camera for skybox
		CAGE_COMPONENT_ENGINE(Transform, t1, engineEntities()->get(1));
		CAGE_COMPONENT_ENGINE(Transform, t2, engineEntities()->get(2));
		t2.orientation = t1.orientation;
	}

	{ // update shadowmaps
		if (*directionalLights)
		{
			Aabb sceneBox = getBoxForScene(1);
			for (Entity *e : directionalLights)
				fitShadowmapForDirectionalLight(e, sceneBox);
		}
	}

	return false;
}

bool guiFunction(uint32 en)
{
	switch (en)
	{
	case 1: // prev
		if (sceneIndexCurrent == 0)
			sceneIndexCurrent = numeric_cast<uint32>(maps.size()) - 1;
		else
			sceneIndexCurrent--;
		return true;
	case 2: // next
		sceneIndexCurrent++;
		if (sceneIndexCurrent == maps.size())
			sceneIndexCurrent = 0;
		return true;
	}

	return false;
}

void cameraInitialize()
{
	cameraCtrl = newFpsCamera();
	cameraCtrl->movementSpeed = 0.5;
	cameraCtrl->mouseButton = MouseButtonsFlags::Left;
	engineProfilingInstance = newEngineProfiling();
}

void updateInitialize()
{
	loadMapsList();
	sceneReload();
	engineAssets()->add(HashString("scenes/common/common.pack"));

	{ // prev
		Entity *a = cage::engineGui()->entities()->createUnique();
		{ // a
			CAGE_COMPONENT_GUI(Scrollbars, sc, a);
			sc.alignment = vec2(0, 1);
		}
		Entity *e = cage::engineGui()->entities()->create(1);
		CAGE_COMPONENT_GUI(Parent, parent, e);
		parent.parent = a->name();
		CAGE_COMPONENT_GUI(Button, c, e);
		CAGE_COMPONENT_GUI(Text, t, e);
		t.value = "< prev";
	}
	{ // next
		Entity *a = cage::engineGui()->entities()->createUnique();
		{ // a
			CAGE_COMPONENT_GUI(Scrollbars, sc, a);
			sc.alignment = vec2(1, 1);
		}
		Entity *e = cage::engineGui()->entities()->create(2);
		CAGE_COMPONENT_GUI(Parent, parent, e);
		parent.parent = a->name();
		CAGE_COMPONENT_GUI(Button, c, e);
		CAGE_COMPONENT_GUI(Text, t, e);
		t.value = "next >";
	}
}

void updateFinalize()
{
	engineProfilingInstance.clear();
	cameraCtrl.clear();
	engineAssets()->remove(HashString("scenes/common/common.pack"));
	if (sceneHashCurrent)
		engineAssets()->remove(sceneHashCurrent);
	sceneHashCurrent = 0;
}
