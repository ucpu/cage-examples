#include "../gui.h"

#include <cage-core/color.h>

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // small
			guiLabel(3, index, "small");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(colorPicker, cp, e);
			cp.collapsible = true;
			cp.color = convertToRainbowColor(randomChance());
		}

		{ // large
			guiLabel(3, index, "large");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(colorPicker, cp, e);
			cp.collapsible = false;
			cp.color = convertToRainbowColor(randomChance());
		}
	}

};

MAIN(guiTestImpl, "color pickers")
