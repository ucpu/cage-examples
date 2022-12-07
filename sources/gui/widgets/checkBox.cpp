#include "../gui.h"

class GuiTestImpl : public GuiTestClass
{
	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>();
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"yes or no, that is the question">();
		}

		{ // with label
			guiLabel(3, index, "with label");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>();
			e->value<GuiTextComponent>().value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Indeterminate;
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
		}

		{ // disabled checked
			guiLabel(3, index, "disabled checked");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Checked;
			e->value<GuiWidgetStateComponent>().disabled = true;
		}
	}
};

MAIN(GuiTestImpl, "check boxes")
