#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	entity *envelopeInScrollbars(entity *e)
	{
		entity *r = gui()->entities()->createUnique();
		CAGE_COMPONENT_GUI(parent, pr, r);
		CAGE_COMPONENT_GUI(parent, pe, e);
		pr = pe;
		pe.parent = r->name();
		pe.order = 0;
		CAGE_COMPONENT_GUI(scrollbars, sc, r);
		return r;
	}

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
			CAGE_COMPONENT_GUI(sliderBar, s, e);
			envelopeInScrollbars(e);
		}

		{ // vertical
			guiLabel(3, index, "vertical");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(sliderBar, s, e);
			s.vertical = true;
			envelopeInScrollbars(e);
		}

		{ // range
			guiLabel(3, index, "range");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(sliderBar, s, e);
			s.min = 13;
			s.max = 42;
			s.value = 21;
			envelopeInScrollbars(e);
		}
	}

};

MAIN(guiTestImpl, "sliders")
