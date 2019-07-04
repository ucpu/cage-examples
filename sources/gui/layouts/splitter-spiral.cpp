#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	entity *makeSplitterRec(uint32 depth)
	{
		entityManager *ents = gui()->entities();

		entity *cell = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(panel, b, cell);
		}

		if (depth == 15)
		{
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = cell->name();
			CAGE_COMPONENT_GUI(label, l, e);
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "Hi";
		}
		else
		{
			CAGE_COMPONENT_GUI(layoutSplitter, s, cell);
			s.vertical = (depth % 2) == 1;
			s.inverse = ((depth / 2) % 2) == 1;
			{ // first
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = cell->name();
				p.order = s.inverse;
				CAGE_COMPONENT_GUI(button, l, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = string() + "Item: " + (depth + 1);
			}
			{ // second
				entity *e = makeSplitterRec(depth + 1);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = cell->name();
				p.order = !s.inverse;
			}
		}

		return cell;
	}

	void initialize() override
	{
		entityManager *ents = gui()->entities();
		entity *e = makeSplitterRec(0);
	}

};

MAIN(guiTestImpl, "splitter spiral")
