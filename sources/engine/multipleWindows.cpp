#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/color.h>
#include <cage-core/macros.h>

#include <cage-engine/window.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <atomic>

using namespace cage;

std::atomic<int> globalWindowIndex;
bool globalClosing = false;
Holder<Mutex> openglInitMut = newMutex();

class windowTestClass
{
public:
	windowTestClass() : index(globalWindowIndex++), hue(index * 0.33), closing(false)
	{
		CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "creating window " + index);
		window = newWindow();
		window->setWindowed();
		window->windowedSize(Vec2i(400, 300));
		{
			ScopeLock l(openglInitMut);
			detail::initializeOpengl();
		}
		window->title(Stringizer() + "window " + index);
		listeners.attachAll(window.get());
#define GCHL_GENERATE(N) listeners.N.bind<windowTestClass, &windowTestClass::N>(this);
		CAGE_EVAL_MEDIUM(CAGE_EXPAND_ARGS(GCHL_GENERATE, windowClose, windowShow, windowHide, windowMove, windowResize, mouseMove, mousePress, mouseDouble, mouseRelease, mouseWheel, focusGain, focusLose, keyPress, keyRelease, keyRepeat, keyChar))
#undef GCHL_GENERATE
		CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "window " + index + " created");
	}

	~windowTestClass()
	{
		CAGE_LOG(SeverityEnum::Info, "test", Stringizer() + "destroying window " + index);
	}

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

	bool windowClose()
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "close window " + index);
		closing = true;
		return true;
	}

	bool windowShow()
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "show window " + index);
		return true;
	}

	bool windowHide()
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "hide window " + index);
		return true;
	}

	bool windowMove(const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "move window " + index + " to " + p[0] + ", " + p[1]);
		return true;
	}

	bool windowResize(const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "resize window " + index + " to " + p[0] + ", " + p[1]);
		return true;
	}

	bool mousePress(MouseButtonsFlags b, ModifiersFlags m, const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse press button " + (uint32)b + ", mods " + (uint32)m + ", at " + p[0] + ", " + p[1] + " in window " + index);
		return true;
	}

	bool mouseDouble(MouseButtonsFlags b, ModifiersFlags m, const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse double click button " + (uint32)b + ", mods " + (uint32)m + ", at " + p[0] + ", " + p[1] + " in window " + index);
		return true;
	}

	bool mouseRelease(MouseButtonsFlags b, ModifiersFlags m, const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse release button " + (uint32)b + ", mods " + (uint32)m + ", at " + p[0] + ", " + p[1] + " in window " + index);
		return true;
	}

	bool mouseMove(MouseButtonsFlags b, ModifiersFlags m, const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse move button " + (uint32)b + ", mods " + (uint32)m + ", to " + p[0] + ", " + p[1] + " in window " + index);
		return true;
	}

	bool mouseWheel(sint32 v, ModifiersFlags m, const Vec2i &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "mouse wheel " + v + ", mods " + (uint32)m + ", at " + p[0] + ", " + p[1] + " in window " + index);
		return true;
	}

	bool focusGain()
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus gain in window " + index);
		return true;
	}

	bool focusLose()
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "focus lost in window " + index);
		return true;
	}

	bool keyPress(uint32 key, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + key + ", mods " + (uint32)m + ", pressed in window " + index);
		return true;
	}

	bool keyRepeat(uint32 key, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + key + ", mods " + (uint32)m + ", repeated in window " + index);
		return true;
	}

	bool keyRelease(uint32 key, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "key " + key + ", mods " + (uint32)m + ", released in window " + index);
		return true;
	}

	bool keyChar(uint32 c)
	{
		CAGE_LOG(SeverityEnum::Info, "event", Stringizer() + "character " + c + " in window " + index);
		return true;
	}

	Holder<Window> window;
	WindowEventListeners listeners;
	const uint32 index;
	Real hue;
	bool closing;
};

void windowThread()
{
	windowTestClass t;
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
