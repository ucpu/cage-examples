#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>
#include <cage-engine/core.h>
#include <cage-engine/window.h>
#include <cage-engine/gui.h>
#include <cage-engine/engine.h>
#include <cage-engine/highPerformanceGpuHint.h>

using namespace cage;

class guiTestClass
{
public:

	static real steeper(rads x)
	{
		return (9 * sin(x) + sin(x * 3)) / 8;
	}

	static void guiLabel(uint32 parentName, uint32 &index, const string &name)
	{
		entityManager *ents = gui()->entities();
		entity *e = ents->createUnique();
		CAGE_COMPONENT_GUI(parent, p, e);
		p.parent = parentName;
		p.order = index++;
		CAGE_COMPONENT_GUI(label, l, e);
		CAGE_COMPONENT_GUI(text, t, e);
		t.value = name;
	}

	static void guiBasicLayout()
	{
		entityManager *ents = gui()->entities();

		{ // splitter
			entity *split = ents->create(1);
			CAGE_COMPONENT_GUI(layoutSplitter, ls, split);
			ls.vertical = true;
		}
		{ // top panel
			entity *panel = ents->create(2);
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = 1;
			p.order = 1;
			CAGE_COMPONENT_GUI(panel, gp, panel);
			CAGE_COMPONENT_GUI(scrollbars, sc, panel);
		}
		{ // bottom panel
			entity *panel = ents->create(3);
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = 1;
			p.order = 2;
			CAGE_COMPONENT_GUI(panel, pan, panel);
			//CAGE_COMPONENT_GUI(layoutTable, lt, panel);
			//lt.vertical = true;
			CAGE_COMPONENT_GUI(scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}
	}

	virtual void windowClose()
	{
		engineStop();
	}

	virtual void update()
	{}

	virtual void guiEvent(uint32 name)
	{
		CAGE_LOG(severityEnum::Info, "gui event", string() + "gui event on entity: " + name);

		entity *e = gui()->entities()->get(name);

		if (e->has(gui()->components().button))
		{
			CAGE_LOG(severityEnum::Info, "gui event", string() + "button press");
		}

		if (e->has(gui()->components().checkBox))
		{
			CAGE_COMPONENT_GUI(checkBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "check box state: " + (uint32)c.state);
		}

		if (e->has(gui()->components().radioBox))
		{
			CAGE_COMPONENT_GUI(radioBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "radio box state: " + (uint32)c.state);
		}

		if (e->has(gui()->components().colorPicker))
		{
			CAGE_COMPONENT_GUI(colorPicker, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "color picker: " + c.color);
		}

		if (e->has(gui()->components().comboBox))
		{
			CAGE_COMPONENT_GUI(comboBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "combo box selected: " + c.selected);
		}

		if (e->has(gui()->components().input))
		{
			CAGE_COMPONENT_GUI(input, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "input box valid: " + c.valid + ", value: " + c.value);
		}

		if (e->has(gui()->components().sliderBar))
		{
			CAGE_COMPONENT_GUI(sliderBar, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "slider bar value: " + c.value);
		}
	}

	virtual void initializeEngine()
	{
		engineInitialize(engineCreateConfig());
	}

	virtual void initialize() = 0;

	int run(const string &title)
	{
		try
		{
			// log to console
			holder<logger> log1 = newLogger();
			log1->format.bind<logFormatConsole>();
			log1->output.bind<logOutputStdOut>();

			initializeEngine();

			// events
			windowCloseListener.attach(window()->events.windowClose);
			windowCloseListener.bind<guiTestClass, &guiTestClass::windowClose>(this);
			updateListener.attach(controlThread().update);
			updateListener.bind<guiTestClass, &guiTestClass::update>(this);
			guiListener.attach(gui()->widgetEvent);
			guiListener.bind<guiTestClass, &guiTestClass::guiEvent>(this);

			// window
			window()->setWindowed();
			window()->windowedSize(ivec2(800, 600));
			window()->title(title);

			// run
			initialize();
			static const uint32 assetsName = hashString("cage-tests/gui/gui.pack");
			assets()->add(assetsName);
			engineStart();
			assets()->remove(assetsName);
			engineFinalize();

			return 0;
		}
		catch (...)
		{
			CAGE_LOG(severityEnum::Error, "test", "caught exception");
			return 1;
		}
	}

	eventListener<void()> windowCloseListener;
	eventListener<void()> updateListener;
	eventListener<void(uint32)> guiListener;

};

#define MAIN(CLASS_NAME, TITLE) int main() { return CLASS_NAME().run(TITLE); }
