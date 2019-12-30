#include <atomic>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/color.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

std::atomic<int> globalWindowIndex;
bool globalClosing = false;
Holder<Mutex> openglInitMut = newMutex();

class windowTestClass
{
public:
	windowTestClass() : index(globalWindowIndex++), hue(index * 0.33), closing(false)
	{
		CAGE_LOG(SeverityEnum::Info, "test", stringizer() + "creating window " + index);
		window = newWindow();
		window->setWindowed();
		window->windowedSize(ivec2(400, 300));
		{
			ScopeLock<Mutex> l(openglInitMut);
			detail::initializeOpengl();
		}
		window->title(stringizer() + "window " + index);
		listeners.attachAll(window.get());
#define GCHL_GENERATE(N) listeners.N.bind<windowTestClass, &windowTestClass::N>(this);
		CAGE_EVAL_MEDIUM(CAGE_EXPAND_ARGS(GCHL_GENERATE, windowClose, windowShow, windowHide, windowMove, windowResize, mouseMove, mousePress, mouseDouble, mouseRelease, mouseWheel, focusGain, focusLose, keyPress, keyRelease, keyRepeat, keyChar))
#undef GCHL_GENERATE
		CAGE_LOG(SeverityEnum::Info, "test", stringizer() + "window " + index + " created");
	}

	~windowTestClass()
	{
		CAGE_LOG(SeverityEnum::Info, "test", stringizer() + "destroying window " + index);
	}

	void tick()
	{
		window->processEvents();
		ivec2 resolution = window->resolution();
		glViewport(0, 0, resolution.x, resolution.y);
		vec3 color = colorHsvToRgb(vec3(hue, 1, 1));
		hue = (hue + 0.002) % 1;
		glClearColor(color[0].value, color[1].value, color[2].value, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		window->swapBuffers();
		threadSleep(5000);
	}

	bool windowClose()
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "close window " + index);
		closing = true;
		return true;
	}

	bool windowShow()
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "show window " + index);
		return true;
	}

	bool windowHide()
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "hide window " + index);
		return true;
	}

	bool windowMove(const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "move window " + index + " to " + p.x + ", " + p.y);
		return true;
	}

	bool windowResize(const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "resize window " + index + " to " + p.x + ", " + p.y);
		return true;
	}

	bool mousePress(MouseButtonsFlags b, ModifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "mouse press button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseDouble(MouseButtonsFlags b, ModifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "mouse double click button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseRelease(MouseButtonsFlags b, ModifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "mouse release button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseMove(MouseButtonsFlags b, ModifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "mouse move button " + (uint32)b + ", mods " + (uint32)m + ", to " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseWheel(sint32 v, ModifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "mouse wheel " + v + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool focusGain()
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "focus gain in window " + index);
		return true;
	}

	bool focusLose()
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "focus lost in window " + index);
		return true;
	}

	bool keyPress(uint32 a, uint32 b, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", pressed in window " + index);
		return true;
	}

	bool keyRepeat(uint32 a, uint32 b, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", repeated in window " + index);
		return true;
	}

	bool keyRelease(uint32 a, uint32 b, ModifiersFlags m)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", released in window " + index);
		return true;
	}

	bool keyChar(uint32 c)
	{
		CAGE_LOG(SeverityEnum::Info, "event", stringizer() + "character " + c + " in window " + index);
		return true;
	}

	Holder<Window> window;
	WindowEventListeners listeners;
	const uint32 index;
	real hue;
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
			thrs[i] = newThread(Delegate<void()>().bind<&windowThread>(), stringizer() + "thread " + i);

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
