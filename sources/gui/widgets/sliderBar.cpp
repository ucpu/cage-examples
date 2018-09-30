#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, s, e);
		}

		{ // vertical
			guiLabel(3, index, "vertical");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, s, e);
			s.vertical = true;
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
		}
	}

};

MAIN(guiTestImpl, "sliders")
