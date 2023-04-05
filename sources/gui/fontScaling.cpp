#include "gui.h"

#include <cage-core/color.h>

const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/regular.ttf",
	"cage/font/ubuntu/bold.ttf",
	"cage/font/ubuntu/italic.ttf",
	"cage/font/ubuntu/monospace.ttf",
};
const String labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu Regular",
	"Ubuntu Bold",
	"Ubuntu Italic",
	"Ubuntu Mono",
};

const uint32 fontsCount = sizeof(fontNames) / sizeof(fontNames[0]);
static_assert(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), "arrays must have same number of elements");

class GuiTestImpl : public GuiTestClass
{
public:
	void update() override
	{
		EntityManager *ents = engineGuiEntities();
		static const Real offset = randomChance() * 1000;
		const Real t = applicationTime() / 2e7f + offset;
		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->get(100 + i);
			e->value<GuiTextFormatComponent>().size = (steeper(Rads(t) + Rads::Full() * Real(i) / fontsCount) * 0.5 + 0.5) * 80 + 10;
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		{
			Entity *panel = ents->create(2);
			panel->value<GuiPanelComponent>();
			panel->value<GuiLayoutScrollbarsComponent>();
			panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5);
			panel->value<GuiLayoutLineComponent>().vertical = true;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			Entity * e = ents->create(100 + i);
			e->value<GuiParentComponent>().parent = 2;
			e->value<GuiParentComponent>().order = i;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = labelTexts[i];
			GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
			format.font = HashString(fontNames[i]);
			format.align = TextAlignEnum::Center;
			format.color = colorHsvToRgb(Vec3(randomChance(), 1, 1));
		}
	}
};

MAIN(GuiTestImpl, "font scaling")
