#include "../gui.h"

using namespace cage;

void tooltipCallback(const GuiTooltipConfig &cfg)
{
	cfg.placement = TooltipPlacementEnum::InvokerCorner;
	auto g = newGuiBuilder(cfg.tooltip);
	auto _1 = g->panel().text("Obi-Wan Kenobi");
	auto _2 = g->column();
	g->label().text("Hello, there!");
	g->label().text("These aren't the droids you're looking for.");
	g->label().text("A great leap forward often requires first taking two steps back.");
	g->label().text("Only a Sith deals in absolutes.");
	g->label().text("I have the high ground.");
}

class GuiTestImpl : public GuiTestClass
{
	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->remove<GuiLayoutAlignmentComponent>();
			e->value<GuiLayoutTableComponent>().sections = 7;
			e->value<GuiLayoutTableComponent>().grid = true;
		}

		for (uint32 i = 0; i < 7 * 7; i++)
		{
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = i;
			e->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5);
			Entity *f = ents->createUnique();
			f->value<GuiParentComponent>().parent = e->name();
			f->value<GuiButtonComponent>();
			f->value<GuiTextComponent>().value = Stringizer() + "button " + i;
			f->value<GuiTooltipComponent>().tooltip = tooltipCallback;
			f->value<GuiExplicitSizeComponent>().size = Vec2(90);
		}
	}
};

MAIN(GuiTestImpl, "tooltips placement buttons")
