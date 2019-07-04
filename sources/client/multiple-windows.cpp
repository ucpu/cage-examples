#include <atomic>

#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/color.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/opengl.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

std::atomic<int> globalWindowIndex;
bool globalClosing = false;
holder<syncMutex> openglInitMut = newSyncMutex();

class windowTestClass
{
public:
	windowTestClass() : index(globalWindowIndex++), hue(index * 0.33), closing(false)
	{
		CAGE_LOG(severityEnum::Info, "test", string() + "creating window " + index);
		window = newWindow();
		window->setWindowed();
		window->windowedSize(ivec2(400, 300));
		{
			scopeLock<syncMutex> l(openglInitMut);
			detail::initializeOpengl();
		}
		window->title(string() + "window " + index);
		listeners.attachAll(window.get());
#define GCHL_GENERATE(N) listeners.N.bind<windowTestClass, &windowTestClass::N>(this);
		CAGE_EVAL_MEDIUM(CAGE_EXPAND_ARGS(GCHL_GENERATE, windowClose, windowShow, windowHide, windowPaint, windowMove, windowResize, mouseMove, mousePress, mouseDouble, mouseRelease, mouseWheel, focusGain, focusLose, keyPress, keyRelease, keyRepeat, keyChar))
#undef GCHL_GENERATE
		CAGE_LOG(severityEnum::Info, "test", string() + "window " + index + " created");
	}

	~windowTestClass()
	{
		CAGE_LOG(severityEnum::Info, "test", string() + "destroying window " + index);
	}

	void tick()
	{
		window->processEvents();
		ivec2 resolution = window->resolution();
		glViewport(0, 0, resolution.x, resolution.y);
		vec3 color = convertHsvToRgb(vec3(hue, 1, 1));
		hue = (hue + 0.002) % 1;
		glClearColor(color[0].value, color[1].value, color[2].value, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		window->swapBuffers();
		threadSleep(5000);
	}

	bool windowClose()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "close window " + index);
		closing = true;
		return true;
	}

	bool windowShow()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "show window " + index);
		return true;
	}

	bool windowHide()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "hide window " + index);
		return true;
	}

	bool windowPaint()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "paint window " + index);
		return true;
	}

	bool windowMove(const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "move window " + index + " to " + p.x + ", " + p.y);
		return true;
	}

	bool windowResize(const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "resize window " + index + " to " + p.x + ", " + p.y);
		return true;
	}

	bool mousePress(mouseButtonsFlags b, modifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "mouse press button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseDouble(mouseButtonsFlags b, modifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "mouse double click button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseRelease(mouseButtonsFlags b, modifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "mouse release button " + (uint32)b + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseMove(mouseButtonsFlags b, modifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "mouse move button " + (uint32)b + ", mods " + (uint32)m + ", to " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool mouseWheel(sint8 v, modifiersFlags m, const ivec2 &p)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "mouse wheel " + v + ", mods " + (uint32)m + ", at " + p.x + ", " + p.y + " in window " + index);
		return true;
	}

	bool focusGain()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "focus gain in window " + index);
		return true;
	}

	bool focusLose()
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "focus lost in window " + index);
		return true;
	}

	bool keyPress(uint32 a, uint32 b, modifiersFlags m)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", pressed in window " + index);
		return true;
	}

	bool keyRepeat(uint32 a, uint32 b, modifiersFlags m)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", repeated in window " + index);
		return true;
	}

	bool keyRelease(uint32 a, uint32 b, modifiersFlags m)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "key " + a + ", scancode " + b + ", mods " + (uint32)m + ", released in window " + index);
		return true;
	}

	bool keyChar(uint32 c)
	{
		CAGE_LOG(severityEnum::Info, "event", string() + "character " + c + " in window " + index);
		return true;
	}

	holder<windowHandle> window;
	windowEventListeners listeners;
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
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		CAGE_LOG(severityEnum::Warning, "compatibility", "be warned, this example depends on undocumented behavior and may not work on some platforms");
		CAGE_LOG(severityEnum::Info, "compatibility", "it is recommended that all window-related tasks are made in the main threadHandle");

		holder<threadHandle> thrs[3];
		for (uint32 i = 0; i < 3; i++)
			thrs[i] = newThread(delegate<void()>().bind<&windowThread>(), string("threadHandle ") + i);

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
