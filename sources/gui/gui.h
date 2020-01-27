#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>

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
		EntityManager *ents = engineGui()->entities();
		Entity *e = ents->createUnique();
		CAGE_COMPONENT_GUI(Parent, p, e);
		p.parent = parentName;
		p.order = index++;
		CAGE_COMPONENT_GUI(Label, l, e);
		CAGE_COMPONENT_GUI(Text, t, e);
		t.value = name;
	}

	static void guiBasicLayout()
	{
		EntityManager *ents = engineGui()->entities();

		{ // splitter
			Entity *split = ents->create(1);
			CAGE_COMPONENT_GUI(LayoutSplitter, ls, split);
			ls.vertical = true;
		}
		{ // top panel
			Entity *panel = ents->create(2);
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = 1;
			p.order = 1;
			CAGE_COMPONENT_GUI(Panel, gp, panel);
			CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
		}
		{ // bottom panel
			Entity *panel = ents->create(3);
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = 1;
			p.order = 2;
			CAGE_COMPONENT_GUI(Panel, pan, panel);
			//CAGE_COMPONENT_GUI(LayoutTable, lt, panel);
			//lt.vertical = true;
			CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
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
		CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "gui event on entity: " + name);

		Entity *e = engineGui()->entities()->get(name);

		if (e->has(engineGui()->components().Button))
		{
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "button press");
		}

		if (e->has(engineGui()->components().CheckBox))
		{
			CAGE_COMPONENT_GUI(CheckBox, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "check box state: " + (uint32)c.state);
		}

		if (e->has(engineGui()->components().RadioBox))
		{
			CAGE_COMPONENT_GUI(RadioBox, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "radio box state: " + (uint32)c.state);
		}

		if (e->has(engineGui()->components().ColorPicker))
		{
			CAGE_COMPONENT_GUI(ColorPicker, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "color picker: " + c.color);
		}

		if (e->has(engineGui()->components().ComboBox))
		{
			CAGE_COMPONENT_GUI(ComboBox, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "combo box selected: " + c.selected);
		}

		if (e->has(engineGui()->components().Input))
		{
			CAGE_COMPONENT_GUI(Input, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "input box valid: " + c.valid + ", value: " + c.value);
		}

		if (e->has(engineGui()->components().SliderBar))
		{
			CAGE_COMPONENT_GUI(SliderBar, c, e);
			CAGE_LOG(SeverityEnum::Info, "gui event", stringizer() + "slider bar value: " + c.value);
		}
	}

	virtual void initializeEngine()
	{
		engineInitialize(EngineCreateConfig());
	}

	virtual void initialize() = 0;

	int run(const string &title)
	{
		try
		{
			// log to console
			Holder<Logger> log1 = newLogger();
			log1->format.bind<logFormatConsole>();
			log1->output.bind<logOutputStdOut>();

			initializeEngine();

			// events
			windowCloseListener.attach(engineWindow()->events.windowClose);
			windowCloseListener.bind<guiTestClass, &guiTestClass::windowClose>(this);
			updateListener.attach(controlThread().update);
			updateListener.bind<guiTestClass, &guiTestClass::update>(this);
			guiListener.attach(engineGui()->widgetEvent);
			guiListener.bind<guiTestClass, &guiTestClass::guiEvent>(this);

			// window
			engineWindow()->setWindowed();
			engineWindow()->windowedSize(ivec2(800, 600));
			engineWindow()->title(title);

			// run
			initialize();
			static const uint32 assetsName = HashString("cage-tests/gui/gui.pack");
			engineAssets()->add(assetsName);
			engineStart();
			engineAssets()->remove(assetsName);
			engineFinalize();

			return 0;
		}
		catch (...)
		{
			CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
			return 1;
		}
	}

	EventListener<void()> windowCloseListener;
	EventListener<void()> updateListener;
	EventListener<void(uint32)> guiListener;

};

#define MAIN(CLASS_NAME, TITLE) int main() { return CLASS_NAME().run(TITLE); }
