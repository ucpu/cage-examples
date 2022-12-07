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
			e->value<GuiRadioBoxComponent>();
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"this widget is called radio box, yet it has nothing to do with music">();
		}

		{ // with label
			guiLabel(3, index, "with label");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>();
			e->value<GuiTextComponent>().value = "label";
		}

		{ // checked
			guiLabel(3, index, "checked");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>().state = CheckBoxStateEnum::Checked;
		}

		{ // indeterminate
			guiLabel(3, index, "indeterminate");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>().state = CheckBoxStateEnum::Indeterminate;
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
		}

		{ // disabled checked
			guiLabel(3, index, "disabled checked");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>().state = CheckBoxStateEnum::Checked;
			e->value<GuiWidgetStateComponent>().disabled = true;
		}

		{ // group 1
			guiLabel(3, index, "group 1");
			Entity *layout = ents->createUnique();
			GuiParentComponent &p = layout->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			layout->value<GuiLayoutLineComponent>();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = layout->name();
				p.order = index++;
				e->value<GuiRadioBoxComponent>();
				e->value<GuiTextComponent>().value = Stringizer() + i;
			}
		}

		{ // group 2
			guiLabel(3, index, "group 2");
			Entity *layout = ents->createUnique();
			GuiParentComponent &p = layout->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			layout->value<GuiLayoutLineComponent>();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = layout->name();
				p.order = index++;
				e->value<GuiRadioBoxComponent>();
				e->value<GuiTextComponent>().value = Stringizer() + i;
			}
		}
	}
};

MAIN(GuiTestImpl, "radio boxes")
