#include "../gui.h"

using namespace cage;

void tooltipMultiline(const GuiTooltipConfig &cfg)
{
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
}

void tooltipCenteredOverLabel(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::Center;
	cfg.tooltip->value<GuiPanelComponent>();
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = cfg.tooltip->name();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "centered over label";
}

void tooltipCenteredInScreen(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::Manual;
	cfg.tooltip->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5);
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->name();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->name();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "centered in the screen";
}

void tooltipScrollbars(const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	cfg.tooltip->value<GuiPanelComponent>();
	cfg.tooltip->value<GuiLayoutScrollbarsComponent>();
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
}

void tooltipModal(const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->name();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->name();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "modal";
}

void tooltipRecursive(uint32 depth, const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->name();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->name();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = Stringizer() + "depth: " + depth;
	e->value<GuiTooltipComponent>().tooltip.bind<uint32, &tooltipRecursive>(depth + 1);
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
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipMultiline>();
		}

		{ // label center
			guiLabel(3, index, "label center");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipCenteredOverLabel>();
		}

		{ // screen center
			guiLabel(3, index, "screen center");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipCenteredInScreen>();
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
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipScrollbars>();
		}

		{ // modal
			guiLabel(3, index, "modal");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipModal>();
		}

		{ // recursive
			guiLabel(3, index, "recursive");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<uint32, &tooltipRecursive>(1);
		}
	}
};

MAIN(GuiTestImpl, "tooltips")
