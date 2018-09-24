#include "game.h"

holder<cameraControllerClass> cameraController;
holder<engineProfilingClass> engineProfilingInstance;

uint32 sceneIndexCurrent;
uint32 sceneIndexLoaded;
uint32 sceneHashCurrent;
uint32 sceneHashLoaded;

static const uint32 directionalLightsCount = 1;
entityClass *directionalLights[directionalLightsCount + 1];
static const uint32 pointLightsCount = 50;
entityClass *pointLights[pointLightsCount + 1];

std::vector<string> maps;

void loadMapsList()
{
	string root = pathFind("scenes-maps");
	holder<directoryListClass> list = newDirectoryList(root);
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
	try
	{ // load new entities
		holder<fileClass> f = newFile(scenePath, fileMode(true, false));
		string name;
		f->readLine(name);
		sceneHashCurrent = hashString(name.c_str());
		while (f->readLine(name))
		{
			if (name.empty())
				continue;
			entityClass *e = entities()->createAnonymous();
			ENGINE_GET_COMPONENT(render, rs, e);
			rs.object = hashString(name.c_str());
			if (!rs.object)
			{
				CAGE_LOG(severityEnum::Note, "exception", string() + "object: '" + name + "'");
				CAGE_THROW_ERROR(exception, "object has invalid hash");
			}
			ENGINE_GET_COMPONENT(transform, ts, e);
			string posLine;
			f->readLine(posLine);
			ts.position = vec3(posLine);
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
		entityClass *cam = entities()->create(1);
		ENGINE_GET_COMPONENT(transform, t, cam);
		t.position = vec3(0, 10, 30);;
		ENGINE_GET_COMPONENT(camera, c, cam);
		c.ambientLight = vec3(0.1, 0.1, 0.1);
		c.cameraOrder = 2;
		c.clear = cameraClearFlags::None;
		cameraController->setEntity(cam);
	}
	{ // skybox
		entityClass *cam = entities()->create(2);
		ENGINE_GET_COMPONENT(transform, tc, cam);
		(void)tc;
		ENGINE_GET_COMPONENT(camera, c, cam);
		c.ambientLight = vec3(1, 1, 1);
		c.renderMask = 2;
		c.cameraOrder = 1;
		c.near = 0.5;
		c.far = 2;
		entityClass *sky = entities()->create(3);
		ENGINE_GET_COMPONENT(transform, ts, sky);
		(void)ts;
		ENGINE_GET_COMPONENT(render, r, sky);
		ENGINE_GET_COMPONENT(animatedTexture, a, sky);
		(void)a;
		r.object = hashString("scenes/common/skybox.obj");
		r.renderMask = 2;
	}
	for (int i = 0; i < directionalLightsCount; i++)
	{ // directional lights
		directionalLights[i] = entities()->createAnonymous();
		ENGINE_GET_COMPONENT(transform, ts, directionalLights[i]);
		ts.orientation = quat(degs(randomChance() * -20 - 30), degs(i * 360.0f / (float)directionalLightsCount + randomChance() * 180 / (float)directionalLightsCount), degs());
		ENGINE_GET_COMPONENT(light, ls, directionalLights[i]);
		ls.color = vec3(1, 1, 1) * 0.5;
		ls.lightType = lightTypeEnum::Directional;
		ENGINE_GET_COMPONENT(shadowmap, ss, directionalLights[i]);
		ss.worldRadius = vec3(35, 35, 35);
		ss.resolution = 512;
	}
	for (int i = 0; i < pointLightsCount; i++)
	{ // point lights
		pointLights[i] = entities()->createAnonymous();
		ENGINE_GET_COMPONENT(transform, ts, pointLights[i]);
		ts.position = (vec3(randomChance(), randomChance(), randomChance()) * 2 - 1) * 15;
		ENGINE_GET_COMPONENT(light, ls, pointLights[i]);
		ls.color = convertHsvToRgb(vec3(randomChance(), 1, 1));
		ls.attenuation = vec3(0.3, 0, 4);
		ls.lightType = lightTypeEnum::Point;
		//ENGINE_GET_COMPONENT(shadowmap, ss, pointLights[i]);
		//ss.worldRadius = vec3(35, 35, 35);
		//ss.resolution = 1024;
		ENGINE_GET_COMPONENT(render, r, pointLights[i]);
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
	if (sceneIndexCurrent != sceneIndexLoaded)
	{
		sceneReload();
		sceneIndexLoaded = sceneIndexCurrent;
	}

	{ // update camera for skybox
		ENGINE_GET_COMPONENT(transform, t1, entities()->get(1));
		ENGINE_GET_COMPONENT(transform, t2, entities()->get(2));
		t2.orientation = t1.orientation;
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
	cameraController = newCameraController();
	cameraController->movementSpeed = 0.5;
	cameraController->mouseButton = mouseButtonsFlags::Left;
	engineProfilingInstance = newEngineProfiling();
}

void updateInitialize()
{
	loadMapsList();
	sceneReload();
	assets()->add(hashString("scenes/common/common.pack"));

	{ // prev
		entityClass *e = cage::gui()->entities()->create(1);
		GUI_GET_COMPONENT(button, c, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "< prev";
		GUI_GET_COMPONENT(position, p, e);
		p.anchor[1] = 1;
		p.position.values[1] = 1;
		p.position.units[1] = unitEnum::ScreenHeight;
	}
	{ // next
		entityClass *e = cage::gui()->entities()->create(2);
		GUI_GET_COMPONENT(button, c, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "next >";
		GUI_GET_COMPONENT(position, p, e);
		p.anchor[0] = 1;
		p.position.values[0] = 1;
		p.position.units[0] = unitEnum::ScreenWidth;
		p.anchor[1] = 1;
		p.position.values[1] = 1;
		p.position.units[1] = unitEnum::ScreenHeight;
	}
}

void updateFinalize()
{
	engineProfilingInstance.clear();
	cameraController.clear();
	assets()->remove(hashString("scenes/common/common.pack"));
	if (sceneHashLoaded)
		assets()->remove(sceneHashLoaded);
	sceneHashLoaded = 0;
	sceneHashCurrent = 0;
}
