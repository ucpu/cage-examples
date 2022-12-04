#include "../gui.h"

class GuiTestImpl : public GuiTestClass
{
public:
	Entity *makeSplitterRec(uint32 depth)
	{
		EntityManager *ents = engineGuiEntities();

		Entity *cell = ents->createUnique();
		{
			cell->value<GuiPanelComponent>();
		}

		if (depth == 15)
		{
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = cell->name();
			e->value<GuiLabelComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "Hi";
		}
		else
		{
			GuiLayoutSplitterComponent &s = cell->value<GuiLayoutSplitterComponent>();
			s.vertical = (depth % 2) == 1;
			s.inverse = ((depth / 2) % 2) == 1;
			{ // first
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = cell->name();
				p.order = s.inverse;
				e->value<GuiButtonComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + "Item: " + (depth + 1);
			}
			{ // second
				Entity *e = makeSplitterRec(depth + 1);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = cell->name();
				p.order = !s.inverse;
			}
		}

		return cell;
	}

	void initialize() override
	{
		makeSplitterRec(0);
	}
};

MAIN(GuiTestImpl, "splitter spiral")
