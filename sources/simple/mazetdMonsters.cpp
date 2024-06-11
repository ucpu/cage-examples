#include <vector>

#include <cage-core/assetManager.h>
#include <cage-core/color.h>
#include <cage-core/config.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-core/noiseFunction.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/scene.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>
#include <cage-simple/fpsCamera.h>
#include <cage-simple/statisticsGui.h>

using namespace cage;
constexpr uint32 AssetsName = HashString("cage-tests/mazetd/mazetd.pack");

struct Monster
{
	String name;
	uint32 modelName = 0;
	uint32 animationName = 0;
};

std::vector<Monster> generateMonsterProperties()
{
	std::vector<Monster> monsters;

	{
		Monster msp;
		msp.name = "Frog";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Frog.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Frog.glb?Frog_Jump");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Snake";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Snake.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Snake.glb?Snake_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Spider";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Spider.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Spider.glb?Spider_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Wasp";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Wasp.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Wasp.glb?Wasp_Flying");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Stegosaurus";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Stegosaurus.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Stegosaurus.glb?Stegosaurus_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Rat";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Rat.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Rat.glb?Rat_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Velociraptor";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Velociraptor.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Velociraptor.glb?Velociraptor_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Slime";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Slime.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Slime.glb?Slime_Idle");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Triceratops";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Triceratops.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Triceratops.glb?Triceratops_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Bat";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Bat.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Bat.glb?Bat_Flying");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Apatosaurus";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Apatosaurus.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Apatosaurus.glb?Apatosaurus_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Skeleton";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Skeleton.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Skeleton.glb?Skeleton_Running");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Parasaurolophus";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Parasaurolophus.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Parasaurolophus.glb?Parasaurolophus_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Trex";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Trex.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Trex.glb?TRex_Walk");
		monsters.push_back(msp);
	}

	{
		Monster msp;
		msp.name = "Dragon";
		msp.modelName = HashString("cage-tests/mazetd/monsters/Dragon.object");
		msp.animationName = HashString("cage-tests/mazetd/monsters/Dragon.glb?Dragon_Flying");
		monsters.push_back(msp);
	}

	return monsters;
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
		//const auto updateListener = controlThread().update.listen(&update);
		const auto closeListener = engineWindow()->events.listen(inputFilter([](input::WindowClose) { engineStop(); }));

		// window
		engineWindow()->setMaximized();
		engineWindow()->title("mazetd monsters (test skeletal animations import transformation)");

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
			t.orientation = Quat(Degs(-50), Degs(270), Degs());
			t.position = Vec3(0, 2, 0);
			LightComponent &l = e->value<LightComponent>();
			l.lightType = LightTypeEnum::Directional;
			l.color = Vec3(1);
			l.intensity = 1;
			ShadowmapComponent &s = e->value<ShadowmapComponent>();
			s.resolution = 2048;
			s.directionalWorldSize = 15;
		}
		{ // floor
			Entity *e = ents->createUnique();
			e->value<RenderComponent>().object = HashString("cage-tests/skeletons/floor/floor.obj");
			e->value<TransformComponent>().position = Vec3();
		}
		{ // monsters
			uint32 index = 0;
			for (const Monster &mr : generateMonsterProperties())
			{
				Vec3 pos = Vec3(Real(index) - 7, 0, 0);
				{
					Entity *e = ents->createAnonymous();
					e->value<TransformComponent>().position = pos;
					e->value<TransformComponent>().orientation = Quat({}, Degs(180), {});
					e->value<RenderComponent>().object = mr.modelName;
					e->value<SkeletalAnimationComponent>().name = mr.animationName;
				}
				{
					Entity *e = ents->createAnonymous();
					e->value<TransformComponent>().position = pos + Vec3(0, 1, 0);
					e->value<TransformComponent>().scale = 0.1;
					e->value<TextComponent>().value = mr.name;
				}
				index++;
			}
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
