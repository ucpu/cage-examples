#include <cage-core/logger.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/assetManager.h>
#include <cage-engine/window.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/guiComponents.h>
#include <cage-engine/highPerformanceGpuHint.h>

#include <cage-simple/engine.h>

using namespace cage;

class GuiTestClass
{
public:
	static Real steeper(Rads x)
	{
		return (9 * sin(x) + sin(x * 3)) / 8;
	}

	static void guiLabel(uint32 parentName, uint32 &index, const String &name)
	{
		EntityManager *ents = engineGuiEntities();
		Entity *e = ents->createUnique();
		e->value<GuiParentComponent>().parent = parentName;
		e->value<GuiParentComponent>().order = index++;
		e->value<GuiLabelComponent>();
		e->value<GuiTextComponent>().value = name;
	}

	static void guiBasicLayout()
	{
		EntityManager *ents = engineGuiEntities();

		{ // splitter
			Entity *split = ents->create(1);
			split->value<GuiLayoutSplitterComponent>().vertical = true;
		}
		{ // top panel
			Entity *panel = ents->create(2);
			panel->value<GuiParentComponent>().parent = 1;
			panel->value<GuiParentComponent>().order = 1;
			panel->value<GuiPanelComponent>();
			panel->value<GuiScrollbarsComponent>();
		}
		{ // bottom panel
			Entity *panel = ents->create(3);
			panel->value<GuiParentComponent>().parent = 1;
			panel->value<GuiParentComponent>().order = 2;
			panel->value<GuiPanelComponent>();
			panel->value<GuiScrollbarsComponent>().alignment = Vec2(0.5, 0);
		}
	}

	Entity *envelopeInScrollbars(Entity *e)
	{
		Entity *r = engineGuiEntities()->createUnique();
		GuiParentComponent &pr = r->value<GuiParentComponent>();
		GuiParentComponent &pe = e->value<GuiParentComponent>();
		pr = pe;
		pe.parent = r->name();
		pe.order = 0;
		r->value<GuiScrollbarsComponent>();
		return r;
	}

	virtual void windowClose()
	{
		engineStop();
	}

	virtual void update()
	{}

	virtual void guiEvent(InputGuiWidget in)
	{
		CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "gui event on entity: " + in.widget);

		Entity *e = engineGuiEntities()->get(in.widget);

		if (e->has<GuiButtonComponent>())
		{
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "button press");
		}

		if (e->has<GuiCheckBoxComponent>())
		{
			GuiCheckBoxComponent &c = e->value<GuiCheckBoxComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "check box state: " + (uint32)c.state);
		}

		if (e->has<GuiRadioBoxComponent>())
		{
			GuiRadioBoxComponent &c = e->value<GuiRadioBoxComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "radio box state: " + (uint32)c.state);
		}

		if (e->has<GuiColorPickerComponent>())
		{
			GuiColorPickerComponent &c = e->value<GuiColorPickerComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "color picker: " + c.color);
		}

		if (e->has<GuiComboBoxComponent>())
		{
			GuiComboBoxComponent &c = e->value<GuiComboBoxComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "combo box selected: " + c.selected);
		}

		if (e->has<GuiInputComponent>())
		{
			GuiInputComponent &c = e->value<GuiInputComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "input box valid: " + c.valid + ", value: " + c.value);
		}

		if (e->has<GuiSliderBarComponent>())
		{
			GuiSliderBarComponent &c = e->value<GuiSliderBarComponent>();
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "slider bar value: " + c.value);
		}
	}

	virtual void initializeEngine()
	{
		engineInitialize(EngineCreateConfig());
	}

	virtual void initialize() = 0;

	int run(const String &title)
	{
		try
		{
			// log to console
			Holder<Logger> log1 = newLogger();
			log1->format.bind<logFormatConsole>();
			log1->output.bind<logOutputStdOut>();

			initializeEngine();

			// events
			windowCloseListener.attach(engineWindow()->events, -5156);
			windowCloseListener.bind<GuiTestClass, &GuiTestClass::onWindowClose>(this);
			updateListener.attach(controlThread().update);
			updateListener.bind<GuiTestClass, &GuiTestClass::update>(this);
			guiListener.attach(engineGuiManager()->widgetEvent);
			guiListener.bind<GuiTestClass, &GuiTestClass::guiEvent>(this);

			// window
			engineWindow()->windowedSize(Vec2i(800, 600));
			engineWindow()->setMaximized();
			engineWindow()->title(title);

			// run
			initialize();
			static constexpr uint32 assetsName = HashString("cage-tests/gui/gui.pack");
			engineAssets()->add(assetsName);
			engineStart();
			engineAssets()->remove(assetsName);
			engineFinalize();

			return 0;
		}
		catch (...)
		{
			detail::logCurrentCaughtException();
			return 1;
		}
	}

	EventListener<bool(const GenericInput &)> windowCloseListener;
	EventListener<void()> updateListener;
	InputListener<InputClassEnum::GuiWidget, InputGuiWidget> guiListener;

protected:
	bool onWindowClose(const GenericInput &in)
	{
		if (in.type == InputClassEnum::WindowClose)
		{
			windowClose();
			return true;
		}
		return false;
	}
};

#define MAIN(CLASS_NAME, TITLE) int main() { return CLASS_NAME().run(TITLE); }
