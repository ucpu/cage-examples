#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-engine/window.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/scene.h>
#include <cage-engine/texture.h>
#include <cage-engine/opengl.h>

#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

#include <vector>

using namespace cage;
constexpr uint32 assetsName = HashString("cage-tests/room/room.pack");
constexpr uint32 screenName = HashString("cage-tests/room/tvscreen.jpg");

void windowClose(InputWindow)
{
	engineStop();
}

void graphicsInitialize()
{
	Holder<Texture> fabScreenTex = newTexture();
	fabScreenTex->initialize(Vec2i(800, 500), 1, GL_RGB16F);
	fabScreenTex->filters(GL_LINEAR, GL_LINEAR, 16);
	fabScreenTex->wraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	fabScreenTex->setDebugName("fabScreenTex");
	{
		Entity *e = engineEntities()->get(4);
		CameraComponent &c = e->value<CameraComponent>();
		c.target = +fabScreenTex;
	}
	engineAssets()->fabricate<AssetSchemeIndexTexture, Texture>(screenName, std::move(fabScreenTex), "fabricated tv screen");
}

void graphicsFinalize()
{
	engineAssets()->remove(screenName);
}

void update()
{
	uint64 time = engineControlTime();
	TransformComponent &t1 = engineEntities()->get(3)->value<TransformComponent>();
	TransformComponent &t2 = engineEntities()->get(4)->value<TransformComponent>();
	t1.orientation = t2.orientation = Quat(Degs(-25), Degs(sin(Degs(time * 3e-5)) * 20 + 125), Degs());
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
		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();
		EventListener<void()> graphicsInitListener;
		graphicsInitListener.attach(graphicsDispatchThread().initialize);
		graphicsInitListener.bind<&graphicsInitialize>();
		EventListener<void()> graphicsFinisListener;
		graphicsFinisListener.attach(graphicsDispatchThread().finalize);
		graphicsFinisListener.bind<&graphicsFinalize>();

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("render to texture");

		// entities
		EntityManager *ents = engineEntities();
		{ // room
			Entity *e = ents->create(10);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/room/room.object");
			r.sceneMask = 3;
			TransformComponent &t = e->value<TransformComponent>();
			(void)t;
		}
		Entity *eye = nullptr;
		{ // eye
			Entity *e = eye = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 1.7, 0);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.3;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.5;
			c.near = 0.2;
			c.far = 100;
			c.sceneMask = 1;
			c.effects = CameraEffectsFlags::Default;
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/room/eye.obj");
			r.sceneMask = 2;
		}
		{ // camera stand
			Entity *e = ents->create(2);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(2.5, 2.5, -1.3);
			t.orientation = Quat(Degs(), Degs(125), Degs());
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/room/camera.obj?camera_stand");
			r.sceneMask = 1;
		}
		{ // camera lens
			Entity *e = ents->create(3);
			e->value<TransformComponent>().position = Vec3(2.5, 2.5, -1.3);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/room/camera.obj?lens");
			r.sceneMask = 1;
		}
		{ // camera body
			Entity *e = ents->create(4);
			e->value<TransformComponent>().position = Vec3(2.5, 2.5, -1.3);
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage-tests/room/camera.obj?camera");
			r.sceneMask = 1;
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.3;
			c.ambientDirectionalColor = Vec3(1);
			c.ambientDirectionalIntensity = 0.5;
			c.near = 0.2;
			c.far = 100;
			c.sceneMask = 2;
			c.effects = CameraEffectsFlags::AmbientOcclusion;
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(eye);
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.1;
		Holder<StatisticsGui> statistics = newStatisticsGui();

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
