#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	entityClass *makeSplitterRec(uint32 depth)
	{
		entityManagerClass *ents = gui()->entities();

		entityClass *cell = ents->createUnique();
		{
			GUI_GET_COMPONENT(panel, b, cell);
		}

		if (depth == 15)
		{
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = cell->name();
			GUI_GET_COMPONENT(label, l, e);
			GUI_GET_COMPONENT(text, t, e);
			t.value = "Hi";
		}
		else
		{
			GUI_GET_COMPONENT(layoutSplitter, s, cell);
			s.vertical = (depth % 2) == 1;
			s.inverse = ((depth / 2) % 2) == 1;
			{ // first
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = cell->name();
				p.order = s.inverse;
				GUI_GET_COMPONENT(button, l, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = string() + "Item: " + (depth + 1);
			}
			{ // second
				entityClass *e = makeSplitterRec(depth + 1);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = cell->name();
				p.order = !s.inverse;
			}
		}

		return cell;
	}

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();
		entityClass *e = makeSplitterRec(0);
	}

};

MAIN(guiTestImpl, "splitter spiral")
