#include "../gui.h"

#include <cage-core/string.h>

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
	void update() override
	{
		EntityManager *ents = engineGuiEntities();
		static const Real offset = randomChance() * 1000;
		const Real t = applicationTime() / 2e7f + offset;
		for (uint32 i = 0; i < fontsCount; i++)
		{
			const Real ls = max((steeper(Rads(t) + Rads::Full() * Real(i) / fontsCount) * 0.5 + 0.5) * 3, 0);
			ents->get(2000 + i)->value<GuiTextFormatComponent>().lineSpacing = ls;
			ents->get(3000 + i)->value<GuiTextComponent>().value = subString(String(Stringizer() + ls), 0, 5);
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
			panel->value<GuiLayoutTableComponent>().sections = 3;
		}

		for (uint32 i = 0; i < fontsCount; i++)
		{
			{
				Entity *e = ents->createUnique();
				e->value<GuiParentComponent>().parent = 2;
				e->value<GuiParentComponent>().order = i * 3 + 0;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = LabelTexts[i];
				e->value<GuiTextFormatComponent>().font = HashString(FontNames[i]);
			}

			{
				Entity *e = ents->create(1000 + i);
				e->value<GuiParentComponent>().parent = 2;
				e->value<GuiParentComponent>().order = i * 3 + 1;
				e->value<GuiPanelComponent>();
			}

			{
				Entity *e = ents->create(2000 + i);
				e->value<GuiParentComponent>().parent = 1000 + i;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = multilineText;
				e->value<GuiTextFormatComponent>().font = HashString(FontNames[i]);
			}

			{
				Entity *e = ents->create(3000 + i);
				e->value<GuiParentComponent>().parent = 2;
				e->value<GuiParentComponent>().order = i * 3 + 2;
				e->value<GuiLabelComponent>();
			}
		}
	}
};

MAIN(GuiTestImpl, "font line spacing")
