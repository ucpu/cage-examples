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
			e->value<GuiSliderBarComponent>();
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "move me all the way to the right">();
			envelopeInScrollbars(e);
		}

		{ // vertical
			guiLabel(3, index, "vertical");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiSliderBarComponent &s = e->value<GuiSliderBarComponent>();
			s.vertical = true;
			envelopeInScrollbars(e);
		}

		{ // range
			guiLabel(3, index, "range");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiSliderBarComponent &s = e->value<GuiSliderBarComponent>();
			s.min = 13;
			s.max = 42;
			s.value = 21;
			envelopeInScrollbars(e);
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiSliderBarComponent>();
			e->value<GuiWidgetStateComponent>().disabled = true;
			envelopeInScrollbars(e);
		}
	}
};

MAIN(GuiTestImpl, "sliders")
