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
			GUI_GET_COMPONENT(progressBar, s, e);
		}

		{ // show text
			guiLabel(3, index, "show text");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(progressBar, s, e);
			s.progress = 0.33;
			s.showValue = true;
		}

		{ // half
			guiLabel(3, index, "half");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(progressBar, s, e);
			s.progress = 0.5;
		}

		{ // full
			guiLabel(3, index, "full");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(progressBar, s, e);
			s.progress = 1;
		}
	}

};

MAIN(guiTestImpl, "progress bars")
