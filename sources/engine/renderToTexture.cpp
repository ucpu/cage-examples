#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/graphics.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/opengl.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/cameraController.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/room/room.pack");
static const uint32 screenName = hashString("cage-tests/room/tvscreen.jpg");
static const uint32 roomName = hashString("cage-tests/room/room.object");

holder<renderTexture> fabScreenTex;

bool windowClose()
{
	engineStop();
	return false;
}

bool graphicsInitialize()
{
	fabScreenTex = newRenderTexture();
	fabScreenTex->image2d(1920, 1080, GL_RGB16F);
	fabScreenTex->filters(GL_LINEAR, GL_LINEAR, 16);
	fabScreenTex->wraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	fabScreenTex->setDebugName("fabScreenTex");
	assets()->set<assetSchemeIndexRenderTexture>(screenName, fabScreenTex.get());
	{
		entity *e = entities()->get(4);
		CAGE_COMPONENT_ENGINE(camera, c, e);
		c.target = fabScreenTex.get();
	}
	return false;
}

bool graphicsFinalize()
{
	assets()->set<assetSchemeIndexRenderTexture>(screenName, (renderTexture*)nullptr);
	fabScreenTex.clear();
	return false;
}

bool update()
{
	uint64 time = currentControlTime();

	CAGE_COMPONENT_ENGINE(transform, t1, entities()->get(3));
	CAGE_COMPONENT_ENGINE(transform, t2, entities()->get(4));
	t1.orientation = t2.orientation = quat(degs(-25), degs(sin(degs(time * 3e-5)) * 20 + 125), degs());

	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		configSetBool("cage-client.engine.debugRenderMissingMeshes", true);
		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), graphicsInitialize, graphicsDispatchThread().initialize);
		GCHL_GENERATE((), graphicsFinalize, graphicsDispatchThread().finalize);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->setWindowed();
		window()->windowedSize(ivec2(800, 600));
		window()->title("render to texture");

		// screen
		assets()->fabricate(assetSchemeIndexRenderTexture, screenName, "fab tv screen");

		// entities
		entityManager *ents = entities();
		{ // room
			entity *e = ents->create(10);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = roomName;
			r.sceneMask = 3;
			CAGE_COMPONENT_ENGINE(transform, t, e);
			(void)t;
		}
		entity *eye = nullptr;
		{ // eye
			entity *e = eye = ents->create(1);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(0, 1.7, 0);
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.6;
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 3;
			c.sceneMask = 1;
			c.effects = cameraEffectsFlags::CombinedPass;
			c.ssao.worldRadius = 0.05;
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/room/eye.obj");
			r.sceneMask = 2;
		}
		{ // camera stand
			entity *e = ents->create(2);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			t.orientation = quat(degs(), degs(125), degs());
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?camera_stand");
			r.sceneMask = 1;
		}
		{ // camera lens
			entity *e = ents->create(3);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?lens");
			r.sceneMask = 1;
		}
		{ // camera body
			entity *e = ents->create(4);
			CAGE_COMPONENT_ENGINE(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			CAGE_COMPONENT_ENGINE(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?camera");
			r.sceneMask = 1;
			CAGE_COMPONENT_ENGINE(camera, c, e);
			c.ambientLight = vec3(0.7);
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 2;
			c.sceneMask = 2;
			c.effects = cameraEffectsFlags::GeometryPass;
			c.ssao.worldRadius = 0.05;
		}

		holder<cameraController> cameraController = newCameraController(eye);
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.1;
		holder<engineProfiling> engineProfiling = newEngineProfiling();

		assets()->add(assetsName);
		engineStart();
		assets()->remove(assetsName);
		assets()->remove(screenName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
