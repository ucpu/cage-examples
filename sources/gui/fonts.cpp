#include "gui.h"

#include <cage-core/color.h>

const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/Ubuntu-R.ttf",
};
const String labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu",
};

const uint32 fontsCount = sizeof(fontNames) / sizeof(fontNames[0]);
static_assert(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), "arrays must have same number of elements");

class guiTestImpl : public guiTestClass
{
public:

	void update() override
	{
		EntityManager *ents = engineGui()->entities();
		static Real offset = randomChance() * 1000;
		Real t = applicationTime() / 2e7f + offset;
		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->get(100 + i);
			GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
			format.size = (steeper(Rads(t) + Rads::Full() * Real(i) / fontsCount) * 0.5 + 0.5) * 80 + 10;
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		Entity *panel = ents->create(2);
		{
			GuiPanelComponent &gp = panel->value<GuiPanelComponent>();
			GuiScrollbarsComponent &sc = panel->value<GuiScrollbarsComponent>();
			sc.alignment = Vec2(0.5, 0.5);
			GuiLayoutLineComponent &ll = panel->value<GuiLayoutLineComponent>();
			ll.vertical = true;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->create(100 + i);
			GuiParentComponent &parent = e->value<GuiParentComponent>();
			parent.parent = panel->name();
			parent.order = i;
			GuiLabelComponent &label = e->value<GuiLabelComponent>();
			GuiTextComponent &text = e->value<GuiTextComponent>();
			text.value = labelTexts[i];
			GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
			format.font = HashString(fontNames[i]);
			format.align = TextAlignEnum::Center;
			format.lineSpacing = 1;
			format.color = colorHsvToRgb(Vec3(randomChance(), 1, 1));
		}
	}

};

MAIN(guiTestImpl, "fonts")
