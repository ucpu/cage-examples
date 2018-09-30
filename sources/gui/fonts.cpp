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
CAGE_ASSERT_COMPILE(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), arrays_must_have_same_number_of_elements);

class guiTestImpl : public guiTestClass
{
public:

	void update() override
	{
		entityManagerClass *ents = gui()->entities();
		static real offset = randomChance() * 1000;
		real t = getApplicationTime() / 2e7f + offset;
		for (uint32 i = 0; i < fontsCount; i++)
		{
			entityClass * e = ents->get(100 + i);
			GUI_GET_COMPONENT(textFormat, format, e);
			format.size = (steeper(rads(t) + rads::Full * real(i) / fontsCount) * 0.5 + 0.5) * 80 + 10;
		}
	}

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		entityClass *panel = ents->create(2);
		{
			GUI_GET_COMPONENT(panel, gp, panel);
			GUI_GET_COMPONENT(position, pos, panel);
			pos.anchor = vec2(.5, .5);
			pos.position.units[0] = unitEnum::ScreenWidth;
			pos.position.units[1] = unitEnum::ScreenHeight;
			pos.position.value = vec2(.5, .5);
			GUI_GET_COMPONENT(layoutLine, ll, panel);
			ll.vertical = true;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			entityClass * e = ents->create(100 + i);
			GUI_GET_COMPONENT(parent, parent, e);
			parent.parent = panel->name();
			parent.order = i;
			GUI_GET_COMPONENT(label, label, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = labelTexts[i];
			GUI_GET_COMPONENT(textFormat, format, e);
			format.font = hashString(fontNames[i]);
			format.align = textAlignEnum::Center;
			format.lineSpacing = 1;
			format.color = convertHsvToRgb(vec3(randomChance(), 1, 1));
		}
	}

};

MAIN(guiTestImpl, "fonts")
