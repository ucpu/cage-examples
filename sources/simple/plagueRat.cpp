#include <atomic>

#include <cage-core/assetsManager.h>
#include <cage-core/concurrent.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/tasks.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/plagueRat/rat.pack");

namespace
{
	// simulate long-running task
	// if this task is scheduled in a thread that is waiting for its own subtasks, it will block it for long time
	// this could block graphics, which is very undesirable

	std::atomic<int> tasksDone = 7;
	Holder<AsyncTask> task;

	void taskRun(uint32)
	{
		CAGE_LOG(SeverityEnum::Info, "plague-rat", "wasteful task");
		threadSleep(1'000'000);
		tasksDone++;
	}

	void engineUpdate()
	{
		if (tasksDone > 0)
		{
			tasksDone--;
			task = tasksRunAsync("waste", taskRun);
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		initializeConsoleLogger();
		engineInitialize(EngineCreateConfig());

		// events
		const auto updateListener = controlThread().update.listen(engineUpdate);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("plague rat (test custom texture)");

		// entities
		EntityManager *ents = engineEntities();
		{ // camera
			Entity *e = ents->create(1);
			e->value<TransformComponent>().position = Vec3(0, 3, 7);
			e->value<TransformComponent>().orientation = Quat(Degs(-20), Degs(), Degs());
			CameraComponent &c = e->value<CameraComponent>();
			c.near = 0.1;
			c.far = 1000;
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 0.2;
		}
		{ // sun
			Entity *e = ents->createUnique();
			TransformComponent &t = e->value<TransformComponent>();
			t.orientation = Quat(Degs(-60), Degs(270), Degs());
			e->value<LightComponent>().lightType = LightTypeEnum::Directional;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<TransformComponent>().position = Vec3();
			e->value<ModelComponent>().model = HashString("cage-tests/skeletons/floor/floor.obj");
		}
		{ // the rat
			Entity *e = ents->createAnonymous();
			e->value<TransformComponent>().position = Vec3(0, 0, 0);
			e->value<TransformComponent>().orientation = Quat({}, Degs(90), {});
			e->value<ModelComponent>().model = HashString("cage-tests/plagueRat/Rat.object");
			e->value<SkeletalAnimationComponent>().animation = HashString("cage-tests/plagueRat/Rat.glb?Rat_Walk");
		}

		Holder<FpsCamera> cameraCtrl = newFpsCamera(ents->get(1));
		cameraCtrl->mouseButton = MouseButtonsFlags::Left;
		cameraCtrl->movementSpeed = 0.3;
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
