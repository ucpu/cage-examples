#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
		}

		{ // with label
			guiLabel(3, index, "with label");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(CheckBox, cb, e);
			cb.state = CheckBoxStateEnum::Indeterminate;
		}
	}

};

MAIN(guiTestImpl, "check boxes")
