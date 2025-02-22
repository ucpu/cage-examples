#include <cage-core/assetsManager.h>
#include <cage-core/entities.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/guiBuilder.h>
#include <cage-engine/guiManager.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/window.h>
#include <cage-simple/engine.h>

namespace GuiTestClassNS
{
	using namespace cage;

	class GuiTestClass
	{
	public:
		static Real steeper(Rads x) { return (9 * sin(x) + sin(x * 3)) / 8; }

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
				split->value<GuiLayoutLineComponent>().vertical = true;
				split->value<GuiLayoutLineComponent>().last = LineEdgeModeEnum::Flexible;
			}
			{ // top panel
				Entity *panel = ents->create(2);
				panel->value<GuiParentComponent>().parent = 1;
				panel->value<GuiParentComponent>().order = 1;
				panel->value<GuiPanelComponent>();
				panel->value<GuiLayoutScrollbarsComponent>();
				panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0);
			}
			{ // bottom panel
				Entity *panel = ents->create(3);
				panel->value<GuiParentComponent>().parent = 1;
				panel->value<GuiParentComponent>().order = 2;
				panel->value<GuiPanelComponent>();
				panel->value<GuiLayoutScrollbarsComponent>();
				panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5, 0);
			}
		}

		Entity *envelopeInScrollbars(Entity *e)
		{
			Entity *r = engineGuiEntities()->createUnique();
			GuiParentComponent &pr = r->value<GuiParentComponent>();
			GuiParentComponent &pe = e->value<GuiParentComponent>();
			pr = pe;
			pe.parent = r->id();
			pe.order = 0;
			r->value<GuiLayoutScrollbarsComponent>();
			return r;
		}

		virtual void windowClose() { engineStop(); }

		virtual void update() {}

		virtual void guiValueEvent(input::GuiValue in)
		{
			CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "gui event on entity: " + in.entity->id());

			Entity *e = in.entity;

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

		virtual void guiConfirmEvent(input::GuiInputConfirm in) { CAGE_LOG(SeverityEnum::Info, "gui event", Stringizer() + "gui event on entity: " + in.entity->id()); }

		virtual void initializeEngine() { engineInitialize(EngineCreateConfig()); }

		virtual void initialize() = 0;

		int run(const String &title)
		{
			try
			{
				initializeConsoleLogger();
				initializeEngine();

				// events
				const auto windowCloseListener = engineWindow()->events.listen(inputFilter([&](input::WindowClose in) { this->windowClose(); }), -5156);
				const auto updateListener = controlThread().update.listen([&]() { this->update(); });
				const auto guiValueListener = engineGuiManager()->widgetEvent.listen(inputFilter([&](input::GuiValue in) { this->guiValueEvent(in); }));
				const auto guiConfirmListener = engineGuiManager()->widgetEvent.listen(inputFilter([&](input::GuiInputConfirm in) { this->guiConfirmEvent(in); }));

				// window
				engineWindow()->windowedSize(Vec2i(800, 600));
				engineWindow()->setMaximized();
				engineWindow()->title(title);

				// run
				initialize();
				static constexpr uint32 AssetsName = HashString("cage-tests/gui/gui.pack");
				engineAssets()->load(AssetsName);
				engineRun();
				engineAssets()->unload(AssetsName);
				engineFinalize();

				return 0;
			}
			catch (...)
			{
				detail::logCurrentCaughtException();
				return 1;
			}
		}
	};
}

using GuiTestClassNS::GuiTestClass;

#define MAIN(CLASS_NAME, TITLE) \
	int main() \
	{ \
		return CLASS_NAME().run(TITLE); \
	}
