#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assets.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

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
		entityManagerClass *ents = gui()->entities();
		entityClass *e = ents->createUnique();
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = parentName;
		p.order = index++;
		GUI_GET_COMPONENT(label, l, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = name;
	}

	static void guiBasicLayout()
	{
		entityManagerClass *ents = gui()->entities();

		{ // splitter
			entityClass *split = ents->create(1);
			GUI_GET_COMPONENT(layoutSplitter, ls, split);
			ls.vertical = true;
		}
		{ // top panel
			entityClass *panel = ents->create(2);
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = 1;
			p.order = 1;
			GUI_GET_COMPONENT(panel, gp, panel);
			GUI_GET_COMPONENT(scrollbars, sc, panel);
		}
		{ // bottom panel
			entityClass *panel = ents->create(3);
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = 1;
			p.order = 2;
			GUI_GET_COMPONENT(panel, pan, panel);
			//GUI_GET_COMPONENT(layoutTable, lt, panel);
			//lt.vertical = true;
			GUI_GET_COMPONENT(scrollbars, sc, panel);
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

		entityClass *e = gui()->entities()->get(name);

		if (e->has(gui()->components().button))
		{
			CAGE_LOG(severityEnum::Info, "gui event", string() + "button press");
		}

		if (e->has(gui()->components().checkBox))
		{
			GUI_GET_COMPONENT(checkBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "check box state: " + (uint32)c.state);
		}

		if (e->has(gui()->components().radioBox))
		{
			GUI_GET_COMPONENT(radioBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "radio box state: " + (uint32)c.state);
		}

		if (e->has(gui()->components().colorPicker))
		{
			GUI_GET_COMPONENT(colorPicker, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "color picker: " + c.color);
		}

		if (e->has(gui()->components().comboBox))
		{
			GUI_GET_COMPONENT(comboBox, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "combo box selected: " + c.selected);
		}

		if (e->has(gui()->components().input))
		{
			GUI_GET_COMPONENT(input, c, e);
			CAGE_LOG(severityEnum::Info, "gui event", string() + "input box valid: " + c.valid + ", value: " + c.value);
		}

		if (e->has(gui()->components().sliderBar))
		{
			GUI_GET_COMPONENT(sliderBar, c, e);
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
			holder<loggerClass> log1 = newLogger();
			log1->filter.bind<logFilterPolicyPass>();
			log1->format.bind<logFormatPolicyConsole>();
			log1->output.bind<logOutputPolicyStdOut>();

			initializeEngine();

			// events
			windowCloseListener.attach(window()->events.windowClose);
			windowCloseListener.bind<guiTestClass, &guiTestClass::windowClose>(this);
			updateListener.attach(controlThread().update);
			updateListener.bind<guiTestClass, &guiTestClass::update>(this);
			guiListener.attach(gui()->widgetEvent);
			guiListener.bind<guiTestClass, &guiTestClass::guiEvent>(this);

			// window
			window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
			window()->windowedSize(pointStruct(800, 600));
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
