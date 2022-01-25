#include <cage-core/assetManager.h>
#include <cage-core/hashString.h>
#include <cage-core/entities.h>
#include <cage-core/logger.h>
#include <cage-core/debug.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/gamepad.h>
#include <cage-engine/window.h>
#include <cage-engine/scene.h>
#include <cage-simple/statisticsGui.h>
#include <cage-simple/engine.h>

#include <vector>

using namespace cage;

struct Gp : Immovable
{
	Holder<Gamepad> g = newGamepad();
	std::vector<Entity *> bs, as;

	InputListener<InputClassEnum::GamepadConnected, InputGamepadState> connectedListener;
	InputListener<InputClassEnum::GamepadDisconnected, InputGamepadState> disconnectedListener;
	InputListener<InputClassEnum::GamepadPress, InputGamepadKey> pressListener;
	InputListener<InputClassEnum::GamepadRelease, InputGamepadKey> releaseListener;
	InputListener<InputClassEnum::GamepadAxis, InputGamepadAxis> axisListener;

	void connected(InputGamepadState in)
	{
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "connected");
	}

	void disconnected(InputGamepadState in)
	{
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "disconnected");
	}

	void press(InputGamepadKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "press: " + in.key);
	}

	void release(InputGamepadKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "release: " + in.key);
	}

	void axis(InputGamepadAxis in)
	{
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "axis: " + in.axis + ", value: " + in.value);
	}

	Gp()
	{
		connectedListener.attach(g->events);
		disconnectedListener.attach(g->events);
		pressListener.attach(g->events);
		releaseListener.attach(g->events);
		axisListener.attach(g->events);
		connectedListener.bind<Gp, &Gp::connected>(this);
		disconnectedListener.bind<Gp, &Gp::disconnected>(this);
		pressListener.bind<Gp, &Gp::press>(this);
		releaseListener.bind<Gp, &Gp::release>(this);
		axisListener.bind<Gp, &Gp::axis>(this);
		CAGE_LOG(SeverityEnum::Info, "gamepad", Stringizer() + "name: " + g->name());
	}

	~Gp()
	{
		destroy();
	}

	void destroy()
	{
		for (Entity *e : bs)
			e->destroy();
		for (Entity *e : as)
			e->destroy();
		bs.clear();
		as.clear();
	}

	void make(PointerRange<Entity *> es)
	{
		EntityManager *ents = engineEntities();

		for (Entity *&e : es)
		{
			e = ents->createUnique();
			e->value<TransformComponent>();
			RenderComponent &r = e->value<RenderComponent>();
			r.object = HashString("cage/model/fake.obj");
		}
	}

	void update(uint32 index)
	{
		g->processEvents();

		const auto ia = g->axes();
		const auto ib = g->buttons();

		if (ia.size() != as.size() || ib.size() != bs.size())
		{
			destroy();
			as.resize(ia.size());
			bs.resize(ib.size());
			make(as);
			make(bs);
		}

		uint32 x = 0;
		uint32 i = 0;
		for (Entity *e : as)
		{
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(x++, ia[i++] * 0.7 + index * 3, 0);
			t.scale = 0.35;
		}
		i = 0;
		for (Entity *e : bs)
		{
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(x++, index * 3, 0);
			t.scale = ib[i++] ? 0.5 : 0.2;
		}
	}
};

std::vector<Holder<Gp>> gps;

void windowClose(InputWindow)
{
	engineStop();
}

void update()
{
	if (gamepadsAvailable())
		gps.push_back(systemMemory().createHolder<Gp>());

	uint32 index = 0;
	for (auto &g : gps)
		g->update(index++);
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

		EventListener<void()> updateListener;
		updateListener.attach(controlThread().update);
		updateListener.bind<&update>();
		InputListener<InputClassEnum::WindowClose, InputWindow> closeListener;
		closeListener.attach(engineWindow()->events);
		closeListener.bind<&windowClose>();

		engineWindow()->setMaximized();
		engineWindow()->title("gamepads");

		{ // camera
			Entity *e = engineEntities()->create(1);
			CameraComponent &c = e->value<CameraComponent>();
			c.ambientColor = Vec3(1);
			c.ambientIntensity = 1;
			c.cameraType = CameraTypeEnum::Orthographic;
			c.camera.orthographicSize = Vec2(30, 20);
			c.near = -10;
			c.far = 100;
			TransformComponent &t = e->value<TransformComponent>();
			t.position = Vec3(15, 10, 0);
		}

		Holder<StatisticsGui> statistics = newStatisticsGui();

		engineStart();

		gps.clear();

		engineFinalize();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
