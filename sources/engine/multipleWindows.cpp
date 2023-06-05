#include <cage-core/color.h>
#include <cage-core/concurrent.h>
#include <cage-core/logger.h>
#include <cage-core/macros.h>

#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/opengl.h>
#include <cage-engine/window.h>

#include <atomic>

using namespace cage;

std::atomic<int> globalWindowIndex;
bool globalClosing = false;
Holder<Mutex> openglInitMut = newMutex();

class WindowTestClass
{
public:
	WindowTestClass()
	{
		CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "creating window " + index);
		window = newWindow({});
		window->setWindowed();
		window->windowedSize(Vec2i(400, 300));
		window->title(Stringizer() + "window " + index);
		{
			ScopeLock l(openglInitMut);
			detail::initializeOpengl();
		}

		listeners.attach(&dispatchers);
#define GCHL_GENERATE(N) listeners.N.bind([this](auto in) { return this->N(in); });
		CAGE_EVAL_MEDIUM(CAGE_EXPAND_ARGS(GCHL_GENERATE, windowClose, windowShow, windowHide, windowMove, windowResize, mouseMove, mousePress, mouseDoublePress, mouseRelease, mouseWheel, focusGain, focusLose, keyPress, keyRelease, keyRepeat, keyChar));
#undef GCHL_GENERATE
		listener.attach(window->events);
		listener.bind([this](const GenericInput &in) { return this->dispatchers.dispatch(in); });

		CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "window " + index + " created");
	}

	~WindowTestClass() { CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "destroying window " + index); }

	void tick()
	{
		window->processEvents();
		Vec2i resolution = window->resolution();
		glViewport(0, 0, resolution[0], resolution[1]);
		Vec3 color = colorHsvToRgb(Vec3(hue, 1, 1));
		hue = (hue + 0.002) % 1;
		glClearColor(color[0].value, color[1].value, color[2].value, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		window->swapBuffers();
		threadSleep(5000);
	}

	bool windowClose(InputWindow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "close window " + index);
		closing = true;
		return true;
	}

	bool windowShow(InputWindow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "show window " + index);
		return true;
	}

	bool windowHide(InputWindow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "hide window " + index);
		return true;
	}

	bool windowMove(InputWindowValue in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "move window " + index + " to " + in.value);
		return true;
	}

	bool windowResize(InputWindowValue in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "resize window " + index + " to " + in.value);
		return true;
	}

	bool mousePress(InputMouse in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse press button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseDoublePress(InputMouse in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse double click button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseRelease(InputMouse in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse release button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseMove(InputMouse in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse move buttons " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", to " + in.position + " in window " + index);
		return true;
	}

	bool mouseWheel(InputMouseWheel in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse wheel " + in.wheel + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool focusGain(InputWindow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus gain in window " + index);
		return true;
	}

	bool focusLose(InputWindow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus lost in window " + index);
		return true;
	}

	bool keyPress(InputKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", pressed in window " + index);
		return true;
	}

	bool keyRepeat(InputKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", repeated in window " + index);
		return true;
	}

	bool keyRelease(InputKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", released in window " + index);
		return true;
	}

	bool keyChar(InputKey in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "character " + in.key + " in window " + index);
		return true;
	}

	Holder<Window> window;
	InputsDispatchers dispatchers;
	InputsListeners listeners;
	EventListener<bool(const GenericInput &)> listener;
	const uint32 index = globalWindowIndex++;
	Real hue = index * 0.33;
	bool closing = false;
};

void windowThread()
{
	WindowTestClass t;
	while (!t.closing && !globalClosing)
		t.tick();
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		CAGE_LOG(SeverityEnum::Warning, "compatibility", "be warned, this example depends on undocumented behavior and may not work on some platforms");
		CAGE_LOG(SeverityEnum::Info, "compatibility", "it is recommended that all window-related tasks are made in the main thread");

		Holder<Thread> thrs[3];
		for (uint32 i = 0; i < 3; i++)
			thrs[i] = newThread(Delegate<void()>().bind<&windowThread>(), Stringizer() + "thread " + i);

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
