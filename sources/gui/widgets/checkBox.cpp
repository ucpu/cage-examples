#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			GuiLayoutTableComponent &layout = e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
		}

		{ // with label
			guiLabel(3, index, "with label");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = CheckBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
			cb.state = CheckBoxStateEnum::Indeterminate;
		}
	}

};

MAIN(guiTestImpl, "check boxes")
