#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	Entity *envelopeInScrollbars(Entity *e)
	{
		Entity *r = engineGuiEntities()->createUnique();
		GuiParentComponent &pr = r->value<GuiParentComponent>();
		GuiParentComponent &pe = e->value<GuiParentComponent>();
		pr = pe;
		pe.parent = r->name();
		pe.order = 0;
		r->value<GuiScrollbarsComponent>();
		return r;
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
			e->value<GuiSliderBarComponent>();
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
	}

};

MAIN(guiTestImpl, "sliders")
