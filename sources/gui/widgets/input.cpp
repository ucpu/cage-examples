#include "../gui.h"

using namespace cage;

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
			e->value<GuiInputComponent>();
		}

		{ // integer
			guiLabel(3, index, "integer");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiInputComponent &ib = e->value<GuiInputComponent>();
			ib.type = InputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
		}

		{ // real
			guiLabel(3, index, "real");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiInputComponent &ib = e->value<GuiInputComponent>();
			ib.type = InputTypeEnum::Real;
			ib.min.f = -1;
			ib.max.f = 1;
			ib.step.f = 0.1;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "range -1 .. 1">();
		}

		{ // password
			guiLabel(3, index, "password");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>().type = InputTypeEnum::Password;
			e->value<GuiTextComponent>().value = "password";
		}

		{ // email
			guiLabel(3, index, "email");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>().type = InputTypeEnum::Email;
			e->value<GuiTextComponent>().value = "@";
		}

		{ // url
			guiLabel(3, index, "url");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>().type = InputTypeEnum::Url;
			e->value<GuiTextComponent>().value = "http://";
		}

		{ // disabled no tooltip
			guiLabel(3, index, "disabled forbids tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "this widget may not show tooltip">();
			e->value<GuiTooltipComponent>().enableForDisabled = false;
		}

		{ // disabled with tooltip
			guiLabel(3, index, "disabled shows tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "this widget should show tooltip">();
			e->value<GuiTooltipComponent>().enableForDisabled = true;
		}
	}
};

MAIN(GuiTestImpl, "input fields")
