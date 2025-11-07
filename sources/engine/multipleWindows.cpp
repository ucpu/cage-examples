/*
#include <atomic>

#include <cage-core/color.h>
#include <cage-core/concurrent.h>
#include <cage-core/logger.h>
#include <cage-core/macros.h>
#include <cage-engine/opengl.h>
#include <cage-engine/window.h>

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
		window->setWindowed();
		window->windowedSize(Vec2i(400, 300));
		window->title(Stringizer() + "window " + index);
		{
			ScopeLock l(openglInitMut);
			detail::initializeOpengl();
		}
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

	bool windowClose(input::WindowClose)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "close window " + index);
		closing = true;
		return true;
	}

	bool windowShow(input::WindowShow)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "show window " + index);
		return true;
	}

	bool windowHide(input::WindowHide)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "hide window " + index);
		return true;
	}

	bool windowMove(input::WindowMove in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "move window " + index + " to " + in.position);
		return true;
	}

	bool windowResize(input::WindowResize in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "resize window " + index + " to " + in.size);
		return true;
	}

	bool mousePress(input::MousePress in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse press button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseDoublePress(input::MouseDoublePress in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse double click button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseRelease(input::MouseRelease in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse release button " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool mouseMove(input::MouseMove in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse move buttons " + (uint32)in.buttons + ", mods " + (uint32)in.mods + ", to " + in.position + " in window " + index);
		return true;
	}

	bool mouseWheel(input::MouseWheel in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse wheel " + in.wheel + ", mods " + (uint32)in.mods + ", at " + in.position + " in window " + index);
		return true;
	}

	bool focusGain(input::WindowFocusGain)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus gain in window " + index);
		return true;
	}

	bool focusLose(input::WindowFocusLose)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus lost in window " + index);
		return true;
	}

	bool keyPress(input::KeyPress in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", pressed in window " + index);
		return true;
	}

	bool keyRepeat(input::KeyRepeat in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", repeated in window " + index);
		return true;
	}

	bool keyRelease(input::KeyRelease in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + in.key + ", mods " + (uint32)in.mods + ", released in window " + index);
		return true;
	}

	bool keyChar(input::Character in)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "character " + in.character + " in window " + index);
		return true;
	}

	Holder<Window> window = newWindow({});
	const uint32 index = globalWindowIndex++;
	Real hue = index * 0.33;
	bool closing = false;

#define GCHL_GENERATE(N) EventListener<bool(const GenericInput &)> listener##N = window->events.listen(inputFilter([this](typename privat::ExtractParam<decltype(&WindowTestClass::N)>::Param in) { this->N(in); }));
	CAGE_EVAL(CAGE_EXPAND_ARGS(GCHL_GENERATE, windowClose, windowShow, windowHide, windowMove, windowResize, focusLose, focusGain));
	CAGE_EVAL(CAGE_EXPAND_ARGS(GCHL_GENERATE, mouseMove, mousePress, mouseDoublePress, mouseRelease, mouseWheel, keyPress, keyRelease, keyRepeat, keyChar));
#undef GCHL_GENERATE
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
*/

int main(int argc, char *args[]) {}
