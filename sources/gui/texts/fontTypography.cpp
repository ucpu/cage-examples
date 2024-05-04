#include "../gui.h"

using namespace cage;

const char *FontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/regular.ttf",
	"cage/font/ubuntu/bold.ttf",
	"cage/font/ubuntu/italic.ttf",
	"cage/font/ubuntu/monospace.ttf",
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
};
const String multilineText = "lorem ipsum dolor sit amet\npříliš žluťoučký kůň úpěl ďábelské ódy\na quick brown fox jumps over a lazy dog\n\nhello world, foo, bar, baz";

const uint32 fontsCount = sizeof(FontNames) / sizeof(FontNames[0]);
static_assert(sizeof(FontNames) / sizeof(FontNames[0]) == sizeof(LabelTexts) / sizeof(LabelTexts[0]), "arrays must have same number of elements");

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		{
			Entity *panel = ents->create(2);
			panel->value<GuiPanelComponent>();
			panel->value<GuiLayoutScrollbarsComponent>();
			panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5);
			panel->value<GuiLayoutTableComponent>().sections = 2;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			{
				Entity *e = ents->createUnique();
				e->value<GuiParentComponent>().parent = 2;
				e->value<GuiParentComponent>().order = i * 2 + 0;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = LabelTexts[i];
				e->value<GuiTextFormatComponent>().font = HashString(FontNames[i]);
			}

			{
				Entity *e = ents->create(1000 + i);
				e->value<GuiParentComponent>().parent = 2;
				e->value<GuiParentComponent>().order = i * 2 + 1;
				e->value<GuiPanelComponent>();
			}

			{
				Entity *e = ents->createUnique();
				e->value<GuiParentComponent>().parent = 1000 + i;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = multilineText;
				e->value<GuiTextFormatComponent>().font = HashString(FontNames[i]);
			}
		}
	}
};

MAIN(GuiTestImpl, "font typography")
