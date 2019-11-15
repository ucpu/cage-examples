#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // empty
			guiLabel(3, index, "empty");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(comboBox, cb, e);
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "placeholder";
		}
		{ // items
			guiLabel(3, index, "items");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(comboBox, cb, e);
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				entity *o = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, o);
				p.parent = e->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(text, t, o);
				t.value = stringizer() + "option " + i;
			}
		}
		{ // preselected
			guiLabel(3, index, "preselected");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(comboBox, cb, e);
			cb.selected = 2;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				entity *o = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, o);
				p.parent = e->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(text, t, o);
				t.value = stringizer() + "option " + i;
			}
		}
	}

};

MAIN(guiTestImpl, "combo boxes")
