#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // empty
			guiLabel(3, index, "empty");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(ComboBox, cb, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "placeholder";
		}
		{ // items
			guiLabel(3, index, "items");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(ComboBox, cb, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, o);
				p.parent = e->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(Text, t, o);
				t.value = stringizer() + "option " + i;
			}
		}
		{ // preselected
			guiLabel(3, index, "preselected");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(ComboBox, cb, e);
			cb.selected = 2;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, o);
				p.parent = e->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(Text, t, o);
				t.value = stringizer() + "option " + i;
			}
		}
	}

};

MAIN(guiTestImpl, "combo boxes")
