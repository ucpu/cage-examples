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

		{ // default
			guiLabel(3, index, "default");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(checkBox, cb, e);
		}

		{ // with label
			guiLabel(3, index, "with label");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(checkBox, cb, e);
			cb.state = checkBoxStateEnum::Indeterminate;
		}
	}

};

MAIN(guiTestImpl, "check boxes")
