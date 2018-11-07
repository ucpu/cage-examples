#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	entityClass *envelopeInScrollbars(entityClass *e)
	{
		entityClass *r = gui()->entities()->createUnique();
		GUI_GET_COMPONENT(parent, pr, r);
		GUI_GET_COMPONENT(parent, pe, e);
		pr = pe;
		pe.parent = r->name();
		pe.order = 0;
		GUI_GET_COMPONENT(scrollbars, sc, r);
		return r;
	}

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		guiBasicLayout();
		{
			entityClass *e = ents->get(3);
			GUI_GET_COMPONENT(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, s, e);
			envelopeInScrollbars(e);
		}

		{ // vertical
			guiLabel(3, index, "vertical");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, s, e);
			s.vertical = true;
			envelopeInScrollbars(e);
		}

		{ // range
			guiLabel(3, index, "range");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, s, e);
			s.min = 13;
			s.max = 42;
			s.value = 21;
			envelopeInScrollbars(e);
		}
	}

};

MAIN(guiTestImpl, "sliders")
