#include "../gui.h"

#include <cage-core/color.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			GuiLayoutTableComponent &layout = e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // small
			guiLabel(3, index, "small");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiColorPickerComponent &cp = e->value<GuiColorPickerComponent>();
			cp.collapsible = true;
			cp.color = colorValueToHeatmapRgb(randomChance());
		}

		{ // large
			guiLabel(3, index, "large");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiColorPickerComponent &cp = e->value<GuiColorPickerComponent>();
			cp.collapsible = false;
			cp.color = colorValueToHeatmapRgb(randomChance());
		}
	}
};

MAIN(GuiTestImpl, "color pickers")
