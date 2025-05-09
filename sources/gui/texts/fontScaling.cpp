#include "../gui.h"

#include <cage-core/color.h>

using namespace cage;

const char *FontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/fonts/ubuntu/regular.ttf",
	"cage/fonts/ubuntu/bold.ttf",
	"cage/fonts/ubuntu/italic.ttf",
	"cage/fonts/ubuntu/monospace.ttf",
	"cage-tests/gui/noto/universal.ttf",
};
const String LabelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu Regular",
	"Ubuntu Bold",
	"Ubuntu Italic",
	"Ubuntu Mono",
	"Noto Universal",
};

const uint32 fontsCount = sizeof(FontNames) / sizeof(FontNames[0]);
static_assert(sizeof(FontNames) / sizeof(FontNames[0]) == sizeof(LabelTexts) / sizeof(LabelTexts[0]), "arrays must have same number of elements");

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
			Entity *e = ents->get(100 + i);
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
			Entity *e = ents->create(100 + i);
			e->value<GuiParentComponent>().parent = 2;
			e->value<GuiParentComponent>().order = i;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = LabelTexts[i];
			GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
			format.font = HashString(FontNames[i]);
			format.align = TextAlignEnum::Center;
			format.color = colorHsvToRgb(Vec3(randomChance(), 1, 1));
		}
	}
};

MAIN(GuiTestImpl, "font scaling")
