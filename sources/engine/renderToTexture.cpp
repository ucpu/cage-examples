#include <vector>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assets.h>
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

holder<textureClass> fabScreenTex;

bool windowClose()
{
	engineStop();
	return false;
}

bool graphicsInitialize()
{
	fabScreenTex = newTexture();
	fabScreenTex->image2d(1000, 600, GL_RGB16F);
	fabScreenTex->filters(GL_LINEAR, GL_LINEAR, 0);
	fabScreenTex->wraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	assets()->set<assetSchemeIndexTexture>(screenName, fabScreenTex.get());
	{
		entityClass *e = entities()->get(4);
		ENGINE_GET_COMPONENT(camera, c, e);
		c.target = fabScreenTex.get();
	}
	return false;
}

bool graphicsFinalize()
{
	assets()->set<assetSchemeIndexTexture>(screenName, (textureClass*)nullptr);
	fabScreenTex.clear();
	return false;
}

bool update()
{
	uint64 time = currentControlTime();

	ENGINE_GET_COMPONENT(transform, t1, entities()->get(3));
	ENGINE_GET_COMPONENT(transform, t2, entities()->get(4));
	t1.orientation = t2.orientation = quat(degs(-25), degs(sin(degs(time * 3e-5)) * 20 + 125), degs());

	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

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
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("render to texture");

		// screen
		assets()->fabricate(assetSchemeIndexTexture, screenName, "fab tv screen");

		// entities
		entityManagerClass *ents = entities();
		{ // room
			entityClass *e = ents->create(10);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = roomName;
			r.renderMask = 3;
			ENGINE_GET_COMPONENT(transform, t, e);
			(void)t;
		}
		entityClass *eye = nullptr;
		{ // eye
			entityClass *e = eye = ents->create(1);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(0, 1.7, 0);
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.6;
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 3;
			c.renderMask = 1;
			c.effects = cameraEffectsFlags::CombinedPass;
			c.ssao.worldRadius = 0.3;
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/room/eye.obj");
			r.renderMask = 2;
		}
		{ // camera stand
			entityClass *e = ents->create(2);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			t.orientation = quat(degs(), degs(125), degs());
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?camera_stand");
			r.renderMask = 1;
		}
		{ // camera lens
			entityClass *e = ents->create(3);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?lens");
			r.renderMask = 1;
		}
		{ // camera body
			entityClass *e = ents->create(4);
			ENGINE_GET_COMPONENT(transform, t, e);
			t.position = vec3(2.5, 2.5, -1.3);
			ENGINE_GET_COMPONENT(render, r, e);
			r.object = hashString("cage-tests/room/camera.obj?camera");
			r.renderMask = 1;
			ENGINE_GET_COMPONENT(camera, c, e);
			c.ambientLight = vec3(1, 1, 1) * 0.7;
			c.near = 0.2;
			c.far = 100;
			c.cameraOrder = 2;
			c.renderMask = 2;
			c.effects = cameraEffectsFlags::GeometryPass;
			c.ssao.worldRadius = 0.3;
		}
		holder<cameraControllerClass> cameraController = newCameraController(eye);
		cameraController->mouseButton = mouseButtonsFlags::Left;
		cameraController->movementSpeed = 0.1;
		holder<engineProfilingClass> engineProfiling = newEngineProfiling();

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
