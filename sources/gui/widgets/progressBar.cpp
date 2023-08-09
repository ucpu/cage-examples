#include "../gui.h"

#include <cage-core/entitiesVisitor.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
	void update() override
	{
		entitiesVisitor([](Entity *e, GuiProgressBarComponent &p) { p.progress = (p.progress + 0.003) % 1; }, engineGuiEntities(), false);
	}

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
			e->value<GuiProgressBarComponent>();
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"some progress is being made">();
		}

		{ // progressed
			guiLabel(3, index, "progressed");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiProgressBarComponent>().progress = 0.5;
		}

		{ // with text
			guiLabel(3, index, "with text");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiProgressBarComponent>();
			e->value<GuiTextComponent>().value = "hello";
		}

		{ // with value
			guiLabel(3, index, "with value");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiProgressBarComponent>().showValue = true;
			e->value<GuiTextComponent>().value = "value";
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiProgressBarComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
		}
	}
};

MAIN(GuiTestImpl, "progress bars")
