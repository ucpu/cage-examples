#include "../gui.h"

using namespace cage;

void tooltipMultiline(const GuiTooltipConfig &cfg)
{
	cfg.tooltip->value<GuiPanelComponent>();
	cfg.tooltip->value<GuiLayoutLineComponent>().vertical = true;
	for (uint32 i = 0; i < 4; i++)
	{
		Entity *e = cfg.tooltip->manager()->createUnique();
		e->value<GuiParentComponent>().parent = cfg.tooltip->id();
		e->value<GuiParentComponent>().order = i;
		e->value<GuiLabelComponent>();
		e->value<GuiTextComponent>().value = Stringizer() + "text " + i;
	}
}

void tooltipCenteredOverLabel(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::InvokerCenter;
	cfg.tooltip->value<GuiPanelComponent>();
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = cfg.tooltip->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "centered over label";
}

void tooltipCenteredInScreen(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::ScreenCenter;
	cfg.tooltip->value<GuiPanelComponent>();
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = cfg.tooltip->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "centered in screen";
}

void tooltipCenteredAtCursor(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::CursorCenter;
	cfg.tooltip->value<GuiPanelComponent>();
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = cfg.tooltip->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "centered at cursor";
}

void tooltipManualPosition(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::Manual;
	cfg.tooltip->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.25);
	cfg.tooltip->value<GuiParentComponent>().order = 100;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->id();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "manual position in top-left quarter";
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
		e->value<GuiParentComponent>().parent = cfg.tooltip->id();
		e->value<GuiParentComponent>().order = i;
		e->value<GuiLabelComponent>();
		e->value<GuiTextComponent>().value = Stringizer() + "text " + i;
	}
}

void tooltipModal(const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->id();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "modal";
}

void tooltipRecursive(uint32 depth, const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Holder g = newGuiBuilder(cfg.tooltip);
	auto _1 = g->panel().size(Vec2(150));
	auto _2 = g->column();
	g->label().text(Stringizer() + "depth: " + depth).tooltip({ Delegate<void(const GuiTooltipConfig &)>().bind<uint32, &tooltipRecursive>(depth + 1) });
	const uint32 cnt = randomRange(0, 5);
	for (uint32 i = 0; i < cnt; i++)
		g->label().text("haha");
}

void tooltipTall(const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->id();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(120, 1000);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "modal";
}

void tooltipWide(const GuiTooltipConfig &cfg)
{
	cfg.closeCondition = TooltipCloseConditionEnum::Modal;
	Entity *f = cfg.tooltip->manager()->createUnique();
	f->value<GuiParentComponent>().parent = cfg.tooltip->id();
	f->value<GuiPanelComponent>();
	f->value<GuiExplicitSizeComponent>().size = Vec2(1000, 120);
	Entity *e = cfg.tooltip->manager()->createUnique();
	e->value<GuiParentComponent>().parent = f->id();
	e->value<GuiLabelComponent>();
	e->value<GuiTextComponent>().value = "modal";
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
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "How are you doing?">();
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

		{ // at cursor
			guiLabel(3, index, "cursor center");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipCenteredAtCursor>();
		}

		{ // manual position
			guiLabel(3, index, "manual position");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipManualPosition>();
		}

		{ // short delay
			guiLabel(3, index, "short delay");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "tooltip">();
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
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "tooltip">();
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
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "tooltip">();
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
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "tooltip">();
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

		{ // tall
			guiLabel(3, index, "tall");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipTall>();
		}

		{ // wide
			guiLabel(3, index, "wide");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "label";
			e->value<GuiTooltipComponent>().tooltip.bind<tooltipWide>();
		}
	}
};

MAIN(GuiTestImpl, "tooltips")
