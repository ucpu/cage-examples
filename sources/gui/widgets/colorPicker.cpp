#include "../gui.h"

#include <cage-core/color.h>

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // small
			guiLabel(3, index, "small");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(colorPicker, cp, e);
			cp.collapsible = true;
			cp.color = convertToRainbowColor(randomChance());
		}

		{ // large
			guiLabel(3, index, "large");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(colorPicker, cp, e);
			cp.collapsible = false;
			cp.color = convertToRainbowColor(randomChance());
		}
	}

};

MAIN(guiTestImpl, "color pickers")
