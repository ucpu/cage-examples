/*

#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/opengl.h>
#include <cage-engine/scene.h>
#include <cage-engine/sceneScreenSpaceEffects.h>
#include <cage-engine/texture.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/room/room.pack");
constexpr uint32 ScreenName = HashString("cage-tests/room/tvscreen.jpg");

const auto graphicsInitListener = graphicsDispatchThread().initialize.listen(
	[]()
	{
		Holder<Texture> fabScreenTex = newTexture();
		fabScreenTex->initialize(Vec2i(800, 500), 1, GL_RGB16F);
		fabScreenTex->filters(GL_LINEAR, GL_LINEAR, 16);
		fabScreenTex->wraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		fabScreenTex->setDebugName("fabScreenTex");
		{
			Entity *e = engineEntities()->get(5);
			CameraComponent &c = e->value<CameraComponent>();
			c.target = +fabScreenTex;
		}
		engineAssets()->loadValue<AssetSchemeIndexTexture, Texture>(ScreenName, std::move(fabScreenTex), "fabricated tv screen");
	});

const auto graphicsFinisListener = graphicsDispatchThread().finalize.listen([]() { engineAssets()->unload(ScreenName); });

const auto updateListener = controlThread().update.listen(
	[]()
	{
		{ // eye
			engineEntities()->get(2)->value<TransformComponent>() = engineEntities()->get(1)->value<TransformComponent>();
		}
		{ // camera
			const uint64 time = engineControlTime();
			const Quat q = Quat(Degs(-25), Degs(sin(Degs(time * 3e-5)) * 20 + 125), Degs());
			for (uint32 i = 4; i <= 6; i++)
				engineEntities()->get(i)->value<TransformComponent>().orientation = q;
		}
	});

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// window
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));
		engineWindow()->setMaximized();
		engineWindow()->title("render to texture");

		// entities
		EntityManager *ents = engineEntities();
		{ // room
			Entity *e = ents->create(10);
			e->value<ModelComponent>().model = HashString("cage-tests/room/room.object");
			e->value<SceneComponent>().sceneMask = 3;
			e->value<TransformComponent>();
		}
		{ // eye (camera)
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 1.7, 0);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.7;
			c.near = 0.2;
			c.far = 100;
			e->value<SceneComponent>().sceneMask = 1;
		}
		{ // eye (model)
			Entity *e = ents->create(2);
			e->value<TransformComponent>();
			e->value<ModelComponent>().model = HashString("cage-tests/room/eye.obj");
			e->value<SceneComponent>().sceneMask = 2;
		}
		{ // camera stand
			Entity *e = ents->create(3);
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(2.5, 2.5, -1.3);
			t.orientation = Quat(Degs(), Degs(125), Degs());
			e->value<ModelComponent>().model = HashString("cage-tests/room/camera.obj?camera_stand");
		}
		{ // camera lens
			Entity *e = ents->create(4);
			e->value<TransformComponent>().position = Vec3(2.5, 2.5, -1.3);
			e->value<ModelComponent>().model = HashString("cage-tests/room/camera.obj?lens");
		}
		{ // camera body (camera)
			Entity *e = ents->create(5);
			e->value<TransformComponent>().position = Vec3(2.5, 2.5, -1.3);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.7;
			c.near = 0.2;
			c.far = 100;
			e->value<ScreenSpaceEffectsComponent>().effects = ScreenSpaceEffectsFlags::AmbientOcclusion;
			e->value<SceneComponent>().sceneMask = 2;
		}
		{ // camera body (model)
			Entity *e = ents->create(6);
			e->value<TransformComponent>().position = Vec3(2.5, 2.5, -1.3);
			e->value<ModelComponent>().model = HashString("cage-tests/room/camera.obj?camera");
		}

		Holder<FpsCamera> fpsCamera = newFpsCamera(ents->get(1));
		fpsCamera->mouseButton = MouseButtonsFlags::Left;
		fpsCamera->movementSpeed = 0.1;
		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineAssets()->load(AssetsName);
		engineRun();
		engineAssets()->unload(AssetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}

*/

int main() {}
