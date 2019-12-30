#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	Entity *envelopeInScrollbars(Entity *e)
	{
		Entity *r = gui()->entities()->createUnique();
		CAGE_COMPONENT_GUI(Parent, pr, r);
		CAGE_COMPONENT_GUI(Parent, pe, e);
		pr = pe;
		pe.parent = r->name();
		pe.order = 0;
		CAGE_COMPONENT_GUI(Scrollbars, sc, r);
		return r;
	}

	void initialize() override
	{
		EntityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(SliderBar, s, e);
			envelopeInScrollbars(e);
		}

		{ // vertical
			guiLabel(3, index, "vertical");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(SliderBar, s, e);
			s.vertical = true;
			envelopeInScrollbars(e);
		}

		{ // range
			guiLabel(3, index, "range");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(SliderBar, s, e);
			s.min = 13;
			s.max = 42;
			s.value = 21;
			envelopeInScrollbars(e);
		}
	}

};

MAIN(guiTestImpl, "sliders")
