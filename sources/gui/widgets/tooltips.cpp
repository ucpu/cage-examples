#include "../gui.h"

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

		{ // default tooltip text
			guiLabel(3, index, "default tooltip text");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"How are you doing?">();
		}

		{ // multiline tooltip
			guiLabel(3, index, "multiline tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<+[](const GuiTooltipConfig &cfg) -> void {
				cfg.tooltip->value<GuiPanelComponent>();
				cfg.tooltip->value<GuiLayoutLineComponent>().vertical = true;
				for (uint32 i = 0; i < 4; i++)
				{
					Entity *e = cfg.tooltip->manager()->createUnique();
					e->value<GuiParentComponent>().parent = cfg.tooltip->name();
					e->value<GuiParentComponent>().order = i;
					e->value<GuiLabelComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + "text " + i;
				}
			}>();
		}

		{ // screen center
			guiLabel(3, index, "screen center");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<+[](const GuiTooltipConfig &cfg) -> void {
				cfg.reposition = false;
				cfg.tooltip->value<GuiScrollbarsComponent>().alignment = Vec2(0.5);
				Entity *f = cfg.tooltip->manager()->createUnique();
				f->value<GuiParentComponent>().parent = cfg.tooltip->name();
				f->value<GuiPanelComponent>();
				f->value<GuiExplicitSizeComponent>().size = Vec2(120);
				Entity *e = cfg.tooltip->manager()->createUnique();
				e->value<GuiParentComponent>().parent = f->name();
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = "centered";
			}>();
		}

		{ // short delay
			guiLabel(3, index, "short delay");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"tooltip">();
			e->value<GuiTooltipComponent>().delay = 0;
		}

		{ // long delay
			guiLabel(3, index, "long delay");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"tooltip">();
			e->value<GuiTooltipComponent>().delay = 2000000;
		}

		{ // disabled label, enabled tooltip
			guiLabel(3, index, "disabled label, enabled tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiWidgetStateComponent>().disabled = true;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"tooltip">();
			e->value<GuiTooltipComponent>().enableForDisabled = true;
		}

		{ // disabled label, disabled tooltip
			guiLabel(3, index, "disabled label, disabled tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiWidgetStateComponent>().disabled = true;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"tooltip">();
			e->value<GuiTooltipComponent>().enableForDisabled = false;
		}

		{ // scrollbar
			guiLabel(3, index, "scrollbar");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<+[](const GuiTooltipConfig &cfg) -> void {
				cfg.tooltip->value<GuiPanelComponent>();
				cfg.tooltip->value<GuiScrollbarsComponent>();
				cfg.tooltip->value<GuiExplicitSizeComponent>().size = Vec2(120);
				cfg.tooltip->value<GuiLayoutLineComponent>().vertical = true;
				for (uint32 i = 0; i < 20; i++)
				{
					Entity *e = cfg.tooltip->manager()->createUnique();
					e->value<GuiParentComponent>().parent = cfg.tooltip->name();
					e->value<GuiParentComponent>().order = i;
					e->value<GuiLabelComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + "text " + i;
				}
			}>();
		}

		{ // modal
			guiLabel(3, index, "modal");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind < +[](const GuiTooltipConfig &cfg) -> void {
				cfg.closeCondition = TooltipCloseConditionEnum::Modal;
				Entity *f = cfg.tooltip->manager()->createUnique();
				f->value<GuiParentComponent>().parent = cfg.tooltip->name();
				f->value<GuiPanelComponent>();
				f->value<GuiExplicitSizeComponent>().size = Vec2(120);
				Entity *e = cfg.tooltip->manager()->createUnique();
				e->value<GuiParentComponent>().parent = f->name();
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = "modal";
			} > ();
		}
	}
};

MAIN(GuiTestImpl, "tooltips")
