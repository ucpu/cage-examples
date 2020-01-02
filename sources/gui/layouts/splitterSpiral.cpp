#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	Entity *makeSplitterRec(uint32 depth)
	{
		EntityManager *ents = engineGui()->entities();

		Entity *cell = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Panel, b, cell);
		}

		if (depth == 15)
		{
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = cell->name();
			CAGE_COMPONENT_GUI(Label, l, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "Hi";
		}
		else
		{
			CAGE_COMPONENT_GUI(LayoutSplitter, s, cell);
			s.vertical = (depth % 2) == 1;
			s.inverse = ((depth / 2) % 2) == 1;
			{ // first
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = cell->name();
				p.order = s.inverse;
				CAGE_COMPONENT_GUI(Button, l, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = stringizer() + "Item: " + (depth + 1);
			}
			{ // second
				Entity *e = makeSplitterRec(depth + 1);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = cell->name();
				p.order = !s.inverse;
			}
		}

		return cell;
	}

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();
		Entity *e = makeSplitterRec(0);
	}

};

MAIN(guiTestImpl, "splitter spiral")
