#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>

#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <atomic>

using namespace cage;

std::atomic<bool> dirty;
std::atomic<uint32> camsLayout;
std::atomic<bool> holes;

bool windowClose()
{
	engineStop();
	return false;
}

bool keyPress(uint32, uint32 b, ModifiersFlags modifiers)
{
	switch (b)
	{
	case 65:
		camsLayout = camsLayout == 2 ? 0 : camsLayout + 1;
		dirty = true;
		return true;
	case 66:
		holes = !holes;
		dirty = true;
		return true;
	}
	return false;
}

void box(const vec3 &pos, const quat &rot)
{
	Entity *e = engineEntities()->createAnonymous();
	CAGE_COMPONENT_ENGINE(Transform, t, e);
	CAGE_COMPONENT_ENGINE(Render, r, e);
	t.position = pos;
	t.orientation = rot;
	r.object = HashString("cage/mesh/fake.obj");
	r.sceneMask = 0b111 & ~(holes ? (1 << randomRange(0, 3)) : 0);
}

void letter(char c, const vec3 &pos)
{
	switch (c)
	{
	case 'C':
		for (rads ang = degs(30); ang < degs(360 - 30); ang += degs(30))
			box(
				vec3(cos(ang), sin(ang) * 2, 0) * 3 + pos,
				quat(degs(), degs(), degs(ang))
			);
		break;
	case 'A':
		box(vec3(+0.0, +2, 0) * 3 + pos, quat(degs(), degs(), degs(+00)));
		box(vec3(-.25, +1, 0) * 3 + pos, quat(degs(), degs(), degs(-10)));
		box(vec3(+.25, +1, 0) * 3 + pos, quat(degs(), degs(), degs(+10)));
		box(vec3(-.50, +0, 0) * 3 + pos, quat(degs(), degs(), degs(-10)));
		box(vec3(+.50, +0, 0) * 3 + pos, quat(degs(), degs(), degs(+10)));
		box(vec3(-.75, -1, 0) * 3 + pos, quat(degs(), degs(), degs(-10)));
		box(vec3(-0.0, -1, 0) * 3 + pos, quat(degs(), degs(), degs(+00)));
		box(vec3(+.75, -1, 0) * 3 + pos, quat(degs(), degs(), degs(+10)));
		box(vec3(-1.0, -2, 0) * 3 + pos, quat(degs(), degs(), degs(-10)));
		box(vec3(+1.0, -2, 0) * 3 + pos, quat(degs(), degs(), degs(+10)));
		break;
	case 'G':
		for (rads ang = degs(30); ang < degs(360 - 30); ang += degs(30))
			box(
				vec3(cos(ang), sin(ang) * 2, 0) * 3 + pos,
				quat(degs(), degs(), degs(ang))
			);
		box(vec3(0.25, -0.5, 0) * 3 + pos, quat(degs(), degs(), degs(-20)));
		break;
	case 'E':
		box(vec3(-1, +2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+0, +2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+1, +2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(-1, +1, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(-1, +0, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+0, +0, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+1, +0, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(-1, -1, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(-1, -2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+0, -2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		box(vec3(+1, -2, 0) * 3 + pos, quat(degs(), degs(), degs()));
		break;
	}
}

void regenerate()
{
	EntityManager *ents = engineEntities();
	ents->destroy();

	for (uint32 i = 0; i < 3; i++)
	{ // camera
		Entity *e = ents->create(i + 1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.ambientColor = vec3(0.1);
		c.ambientColor[i] = 0;
		c.sceneMask = 1 << i;
		c.cameraOrder = i;
		switch ((uint32)camsLayout)
		{
		case 0:
			c.viewportOrigin = vec2(i / 3.f, 0);
			c.viewportSize = vec2(1 / 3.f, 1);
			break;
		case 1:
			c.viewportOrigin = vec2(0, i / 3.f);
			c.viewportSize = vec2(1, 1 / 3.f);
			break;
		case 2: // rotating
			break;
		}
		c.effects = CameraEffectsFlags::CombinedPass;
	}

	letter('C', vec3(-3, 0, +0) * 10);
	letter('A', vec3(-1, 0, -1) * 10);
	letter('G', vec3(+1, 0, -1) * 10);
	letter('E', vec3(+3, 0, +0) * 10);

	{ // light
		Entity *e = ents->create(10);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		t.orientation = randomDirectionQuat();
		CAGE_COMPONENT_ENGINE(Light, l, e);
		l.lightType = LightTypeEnum::Directional;
		l.color = vec3(1);
		CAGE_COMPONENT_ENGINE(Shadowmap, s, e);
		s.resolution = 512;
		s.worldSize = vec3(50);
	}
}

bool update()
{
	uint64 time = engineControlTime();

	if (dirty)
	{
		dirty = false;
		regenerate();
	}

	EntityManager *ents = engineEntities();

	if (camsLayout == 2)
	{ // rotating viewports
		for (uint32 i = 0; i < 3; i++)
		{
			Entity *e = ents->get(i + 1);
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.5;
			c.sceneMask = 1 << i;
			c.cameraOrder = i;
			rads ang = degs(20 * time / 1e6 + i * 120);
			c.viewportOrigin = vec2(0.5 + cos(ang) * 0.5, 0.5 + sin(ang) * 0.5) * 0.75;
			c.viewportSize = vec2(0.25, 0.25);
		}
	}

	for (uint32 i = 0; i < 3; i++)
	{ // transformations of cameras
		Entity *e = ents->get(i + 1);
		CAGE_COMPONENT_ENGINE(Transform, t, e);
		switch (i)
		{
		case 0:
			t.orientation = quat(degs(), degs(20 * time / 1e6), degs());
			t.position = vec3(0, 0, 50) * t.orientation;
			break;
		case 1:
			t.orientation = quat(degs(15 * time / 1e6), degs(), degs());
			t.position = vec3(0, 0, 50) * t.orientation;
			break;
		case 2:
			t.orientation = quat(degs(), degs(), degs(10 * time / 1e6));
			t.position = vec3(0, 0, 50) * t.orientation;
			break;
		}
	}

	return false;
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
		EventListener<bool()> applicationQuitListener;
		EventListener<bool()> windowCloseListener;
		EventListener<bool(uint32 key, uint32, ModifiersFlags modifiers)> keyPressListener;
		EventListener<bool()> updateListener;
		windowCloseListener.bind<&windowClose>();
		keyPressListener.bind<&keyPress>();
		updateListener.bind<&update>();
		windowCloseListener.attach(engineWindow()->events.windowClose);
		keyPressListener.attach(engineWindow()->events.keyPress);
		updateListener.attach(controlThread().update);

		engineWindow()->setMaximized();
		engineWindow()->title("multiple viewports");
		dirty = true;
		Holder<EngineProfiling> EngineProfiling = newEngineProfiling();

		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
