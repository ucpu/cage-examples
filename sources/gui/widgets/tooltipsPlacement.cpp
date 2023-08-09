#include "../gui.h"

using namespace cage;

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
			f->value<GuiLabelComponent>();
			f->value<GuiTextComponent>().value = Stringizer() + "label " + i;
			f->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"How are you doing?">();
		}
	}
};

MAIN(GuiTestImpl, "tooltips placement")
