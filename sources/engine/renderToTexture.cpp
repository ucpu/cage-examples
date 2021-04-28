#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/texture.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/opengl.h>
#include <cage-engine/engineProfiling.h>
#include <cage-engine/fpsCamera.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/room/room.pack");
constexpr uint32 screenName = HashString("cage-tests/room/tvscreen.jpg");

bool windowClose()
{
	engineStop();
	return false;
}

bool graphicsInitialize()
{
	Holder<Texture> fabScreenTex = newTexture();
	fabScreenTex->image2d(800, 500, GL_RGB16F);
	fabScreenTex->filters(GL_LINEAR, GL_LINEAR, 16);
	fabScreenTex->wraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	fabScreenTex->setDebugName("fabScreenTex");
	{
		Entity *e = engineEntities()->get(4);
		CAGE_COMPONENT_ENGINE(Camera, c, e);
		c.target = fabScreenTex.get();
	}
	engineAssets()->fabricate<AssetSchemeIndexTexture, Texture>(screenName, templates::move(fabScreenTex), "fabricated tv screen");
	return false;
}

bool graphicsFinalize()
{
	engineAssets()->remove(screenName);
	return false;
}

bool update()
{
	uint64 time = engineControlTime();

	CAGE_COMPONENT_ENGINE(Transform, t1, engineEntities()->get(3));
	CAGE_COMPONENT_ENGINE(Transform, t2, engineEntities()->get(4));
	t1.orientation = t2.orientation = quat(degs(-25), degs(sin(degs(time * 3e-5)) * 20 + 125), degs());

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
#define GCHL_GENERATE(TYPE, FUNC, EVENT) EventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, engineWindow()->events.windowClose);
		GCHL_GENERATE((), graphicsInitialize, graphicsDispatchThread().initialize);
		GCHL_GENERATE((), graphicsFinalize, graphicsDispatchThread().finalize);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("render to texture");

		// entities
		EntityManager *ents = engineEntities();
		{ // room
			Entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/room/room.object");
			r.sceneMask = 3;
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			(void)t;
		}
		Entity *eye = nullptr;
		{ // eye
			Entity *e = eye = ents->create(1);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(0, 1.7, 0);
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.3;
			c.ambientDirectionalColor = vec3(1);
			c.ambientDirectionalIntensity = 0.5;
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 3;
			c.sceneMask = 1;
			c.effects = CameraEffectsFlags::Default;
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/room/eye.obj");
			r.sceneMask = 2;
		}
		{ // camera stand
			Entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			t.orientation = quat(degs(), degs(125), degs());
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/room/camera.obj?camera_stand");
			r.sceneMask = 1;
		}
		{ // camera lens
			Entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/room/camera.obj?lens");
			r.sceneMask = 1;
		}
		{ // camera body
			Entity *e = ents->create(4);
			CAGE_COMPONENT_ENGINE(Transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			CAGE_COMPONENT_ENGINE(Render, r, e);
			r.object = HashString("cage-tests/room/camera.obj?camera");
			r.sceneMask = 1;
			CAGE_COMPONENT_ENGINE(Camera, c, e);
			c.ambientColor = vec3(1);
			c.ambientIntensity = 0.3;
			c.ambientDirectionalColor = vec3(1);
			c.ambientDirectionalIntensity = 0.5;
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 2;
			c.sceneMask = 2;
			c.effects = CameraEffectsFlags::AmbientOcclusion | CameraEffectsFlags::MotionBlur;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(eye);
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.1;
		Holder<EngineProfiling> engineProfiling = newEngineProfiling();

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
