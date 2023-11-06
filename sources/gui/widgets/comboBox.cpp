#include "../gui.h"

using namespace cage;

void tooltipCallback(uint32 index, const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::InvokerCorner;
	auto g = newGuiBuilder(cfg.tooltip);
	auto _ = g->panel();
	g->label().text(Stringizer() + "option " + index);
}

class GuiTestImpl : public GuiTestClass
{
	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // empty
			guiLabel(3, index, "empty");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "placeholder";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"nothing to choose from?">();
		}
		{ // items
			guiLabel(3, index, "items");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				o->value<GuiTextComponent>().value = Stringizer() + "option " + i;
			}
		}
		{ // preselected
			guiLabel(3, index, "preselected");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>().selected = 2;
			e->value<GuiTextComponent>().value = "select one:";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"select one">();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				o->value<GuiTextComponent>().value = Stringizer() + "option " + i;
				o->value<GuiTooltipComponent>().tooltip.bind<uint32, &tooltipCallback>(i);
			}
		}
		{ // disabled combo
			guiLabel(3, index, "disabled combo");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
			e->value<GuiTextComponent>().value = "select one:";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"select one">();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				o->value<GuiTextComponent>().value = Stringizer() + "option " + i;
				o->value<GuiTooltipComponent>().tooltip.bind<uint32, &tooltipCallback>(i);
			}
		}
		{ // disabled items
			guiLabel(3, index, "disabled items");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "select one:";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"select one">();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				o->value<GuiTextComponent>().value = Stringizer() + "option " + i;
				if (i == 2)
					o->value<GuiWidgetStateComponent>().disabled = true;
				o->value<GuiTooltipComponent>().tooltip.bind<uint32, &tooltipCallback>(i);
			}
		}
	}
};

MAIN(GuiTestImpl, "combo boxes")
