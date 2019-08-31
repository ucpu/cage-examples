#include "game.h"

#include <cage-core/geometry.h>
#include <cage-client/shadowmapHelpers.h>

holder<cameraController> cameraCtrl;
holder<engineProfiling> engineProfilingInstance;

uint32 sceneIndexCurrent;
uint32 sceneIndexLoaded;
uint32 sceneHashCurrent;
uint32 sceneHashLoaded;

static const uint32 directionalLightsCount = 1;
entity *directionalLights[directionalLightsCount];
static const uint32 pointLightsCount = 50;
entity *pointLights[pointLightsCount];

std::vector<string> maps;

void loadMapsList()
{
	string root = pathSearchTowardsRoot("scenes-maps", pathTypeFlags::Directory | pathTypeFlags::Archive);
	holder<directoryList> list = newDirectoryList(root);
	while (list->valid())
	{
		maps.push_back(pathJoin(root, list->name()));
		list->next();
	}
	CAGE_LOG(severityEnum::Info, "scenes", string() + "found " + maps.size() + " maps");
}

void sceneReload()
{
	CAGE_LOG(severityEnum::Info, "scenes", string() + "loading scene index: " + sceneIndexCurrent);
	string scenePath = maps[sceneIndexCurrent];
	CAGE_LOG(severityEnum::Info, "scenes", string() + "loading scene description from file: '" + scenePath + "'");
	window()->title(string() + "map: " + pathExtractFilename(scenePath));
	entities()->destroy();

	// load new entities
	try
	{
		holder<fileHandle> f = newFile(scenePath, fileMode(true, false));
		string name;
		f->readLine(name);
		sceneHashCurrent = hashString(name.c_str());
		while (f->readLine(name))
		{
			if (name.empty())
				continue;
			entity *e = entities()->createAnonymous();
			CAGE_COMPONENT_ENGINE(render, rs, e);
			rs.object = hashString(name.c_str());
			if (!rs.object)
			{
				CAGE_LOG(severityEnum::Note, "exception", string() + "object: '" + name + "'");
				CAGE_THROW_ERROR(exception, "object has invalid hash");
			}
			CAGE_COMPONENT_ENGINE(transform, ts, e);
			string posLine;
			f->readLine(posLine);
			ts.position = vec3::parse(posLine);
			string rotLine;
			f->readLine(rotLine);
			ts.orientation = quat(rads(), rads(rotLine.toFloat()), rads());
		}
		CAGE_LOG(severityEnum::Info, "scenes", string() + "scene contains " + (entities()->group()->count()) + " entities");
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "scenes", "failed to load a scene description");
	}

	{ // camera
		entity *cam = entities()->create(1);
		CAGE_COMPONENT_ENGINE(transform, t, cam);
		t.position = vec3(0, 10, 30);
		CAGE_COMPONENT_ENGINE(camera, c, cam);
		c.ambientLight = vec3(0.1);
		c.cameraOrder = 2;
		c.near = 0.1;
		c.far = 200;
		c.clear = cameraClearFlags::None;
		c.effects = cameraEffectsFlags::CombinedPass;
		cameraCtrl->setEntity(cam);
	}

	{ // skybox
		entity *cam = entities()->create(2);
		CAGE_COMPONENT_ENGINE(transform, tc, cam);
		(void)tc;
		CAGE_COMPONENT_ENGINE(camera, c, cam);
		c.ambientLight = vec3(1);
		c.sceneMask = 2;
		c.cameraOrder = 1;
		c.near = 0.5;
		c.far = 2;
		entity *sky = entities()->create(3);
		CAGE_COMPONENT_ENGINE(transform, ts, sky);
		(void)ts;
		CAGE_COMPONENT_ENGINE(render, r, sky);
		CAGE_COMPONENT_ENGINE(textureAnimation, a, sky);
		(void)a;
		r.object = hashString("scenes/common/skybox.obj");
		r.sceneMask = 2;
	}
	
	// directional lights
	for (int i = 0; i < directionalLightsCount; i++)
	{
		directionalLights[i] = entities()->createAnonymous();
		CAGE_COMPONENT_ENGINE(transform, ts, directionalLights[i]);
		ts.orientation = quat(degs(randomChance() * -20 - 30), degs(i * 360.0f / (float)directionalLightsCount + randomChance() * 180 / (float)directionalLightsCount), degs());
		CAGE_COMPONENT_ENGINE(light, ls, directionalLights[i]);
		ls.color = vec3(2.5);
		ls.lightType = lightTypeEnum::Directional;
		CAGE_COMPONENT_ENGINE(shadowmap, ss, directionalLights[i]);
		ss.resolution = 2048;
	}
	
	// point lights
	for (int i = 0; i < pointLightsCount; i++)
	{
		pointLights[i] = entities()->createAnonymous();
		CAGE_COMPONENT_ENGINE(transform, ts, pointLights[i]);
		ts.position = (vec3(randomChance(), randomChance(), randomChance()) * 2 - 1) * 15;
		CAGE_COMPONENT_ENGINE(light, ls, pointLights[i]);
		ls.color = convertHsvToRgb(vec3(randomChance(), 1, 1));
		ls.lightType = lightTypeEnum::Point;
		//CAGE_COMPONENT_ENGINE(shadowmap, ss, pointLights[i]);
		//ss.worldRadius = vec3(35);
		//ss.resolution = 1024;
		CAGE_COMPONENT_ENGINE(render, r, pointLights[i]);
		r.color = ls.color;
		r.object = hashString("scenes/common/lightbulb.obj");
	}
}

bool closeButton()
{
	engineStop();
	return false;
}

bool keyPress(uint32 a, uint32 b, modifiersFlags m)
{
	switch (a)
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
		uint64 now = getApplicationTime();
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
		CAGE_COMPONENT_ENGINE(transform, t1, entities()->get(1));
		CAGE_COMPONENT_ENGINE(transform, t2, entities()->get(2));
		t2.orientation = t1.orientation;
	}

	{ // update shadowmaps
		if (*directionalLights)
		{
			aabb sceneBox = getBoxForRenderScene();
			for (entity *e : directionalLights)
				fitShadowmapForDirectionalLight(e, sceneBox);
		}
	}

	return false;
}

bool assetsUpdate()
{
	if (sceneHashCurrent != sceneHashLoaded)
	{
		if (sceneHashLoaded)
			assets()->remove(sceneHashLoaded);
		sceneHashLoaded = sceneHashCurrent;
		assets()->add(sceneHashLoaded);
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
	cameraCtrl = newCameraController();
	cameraCtrl->movementSpeed = 0.5;
	cameraCtrl->mouseButton = mouseButtonsFlags::Left;
	engineProfilingInstance = newEngineProfiling();
}

void updateInitialize()
{
	loadMapsList();
	sceneReload();
	assets()->add(hashString("scenes/common/common.pack"));

	{ // prev
		entity *a = cage::gui()->entities()->createUnique();
		{ // a
			CAGE_COMPONENT_GUI(scrollbars, sc, a);
			sc.alignment = vec2(0, 1);
		}
		entity *e = cage::gui()->entities()->create(1);
		CAGE_COMPONENT_GUI(parent, parent, e);
		parent.parent = a->name();
		CAGE_COMPONENT_GUI(button, c, e);
		CAGE_COMPONENT_GUI(text, t, e);
		t.value = "< prev";
	}
	{ // next
		entity *a = cage::gui()->entities()->createUnique();
		{ // a
			CAGE_COMPONENT_GUI(scrollbars, sc, a);
			sc.alignment = vec2(1, 1);
		}
		entity *e = cage::gui()->entities()->create(2);
		CAGE_COMPONENT_GUI(parent, parent, e);
		parent.parent = a->name();
		CAGE_COMPONENT_GUI(button, c, e);
		CAGE_COMPONENT_GUI(text, t, e);
		t.value = "next >";
	}
}

void updateFinalize()
{
	engineProfilingInstance.clear();
	cameraCtrl.clear();
	assets()->remove(hashString("scenes/common/common.pack"));
	if (sceneHashLoaded)
		assets()->remove(sceneHashLoaded);
	sceneHashLoaded = 0;
	sceneHashCurrent = 0;
}
