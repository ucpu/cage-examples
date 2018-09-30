#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(radioBox, cb, e);
			cb.group = 1;
		}

		{ // with label
			guiLabel(3, index, "with label");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(radioBox, cb, e);
			cb.group = 1;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(radioBox, cb, e);
			cb.group = 1;
			cb.state = checkBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(radioBox, cb, e);
			cb.group = 1;
			cb.state = checkBoxStateEnum::Indeterminate;
		}
	}

};

MAIN(guiTestImpl, "radio boxes")
