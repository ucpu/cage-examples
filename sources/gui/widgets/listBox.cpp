#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // empty
			guiLabel(3, index, "empty");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(listBox, cb, e);
		}
		{ // items
			guiLabel(3, index, "items");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(listBox, cb, e);
			for (uint32 i = 0; i < 4; i++)
			{
				entityClass *o = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, o);
				p.parent = e->name();
				p.order = index++;
				GUI_GET_COMPONENT(text, t, o);
				t.value = string("option ") + i;
			}
		}
		{ // preselected
			guiLabel(3, index, "preselected");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(listBox, cb, e);
			for (uint32 i = 0; i < 4; i++)
			{
				entityClass *o = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, o);
				p.parent = e->name();
				p.order = index++;
				GUI_GET_COMPONENT(text, t, o);
				t.value = string("option ") + i;
				if (i == 2)
				{
					GUI_GET_COMPONENT(selectedItem, s, o);
				}
			}
		}
	}

};

MAIN(guiTestImpl, "list boxes")
