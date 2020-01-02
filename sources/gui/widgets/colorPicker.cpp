#include "../gui.h"

#include <cage-core/color.h>

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // small
			guiLabel(3, index, "small");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(ColorPicker, cp, e);
			cp.collapsible = true;
			cp.color = colorValueToHeatmapRgb(randomChance());
		}

		{ // large
			guiLabel(3, index, "large");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(ColorPicker, cp, e);
			cp.collapsible = false;
			cp.color = colorValueToHeatmapRgb(randomChance());
		}
	}

};

MAIN(guiTestImpl, "color pickers")
