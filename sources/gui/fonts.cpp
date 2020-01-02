#include "gui.h"

#include <cage-core/color.h>

const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/Ubuntu-R.ttf",
};
const string labelTexts[] = {
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
		static real offset = randomChance() * 1000;
		real t = getApplicationTime() / 2e7f + offset;
		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->get(100 + i);
			CAGE_COMPONENT_GUI(TextFormat, format, e);
			format.size = (steeper(rads(t) + rads::Full() * real(i) / fontsCount) * 0.5 + 0.5) * 80 + 10;
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		Entity *panel = ents->create(2);
		{
			CAGE_COMPONENT_GUI(Panel, gp, panel);
			CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0.5);
			CAGE_COMPONENT_GUI(LayoutLine, ll, panel);
			ll.vertical = true;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->create(100 + i);
			CAGE_COMPONENT_GUI(Parent, parent, e);
			parent.parent = panel->name();
			parent.order = i;
			CAGE_COMPONENT_GUI(Label, label, e);
			CAGE_COMPONENT_GUI(Text, text, e);
			text.value = labelTexts[i];
			CAGE_COMPONENT_GUI(TextFormat, format, e);
			format.font = HashString(fontNames[i]);
			format.align = TextAlignEnum::Center;
			format.lineSpacing = 1;
			format.color = colorHsvToRgb(vec3(randomChance(), 1, 1));
		}
	}

};

MAIN(guiTestImpl, "fonts")
