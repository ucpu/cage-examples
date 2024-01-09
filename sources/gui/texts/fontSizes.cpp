#include "../gui.h"

using namespace cage;

constexpr const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/regular.ttf",
	"cage/font/ubuntu/bold.ttf",
	"cage/font/ubuntu/italic.ttf",
	"cage/font/ubuntu/monospace.ttf",
};
constexpr String labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu Regular",
	"Ubuntu Bold",
	"Ubuntu Italic",
	"Ubuntu Mono",
};
constexpr String multilineText = "lorem ipsum dolor sit amet\npříliš žluťoučký kůň úpěl ďábelské ódy\na quick brown fox jumps over a lazy dog\nhello world, foo, bar, baz\n\n";

const uint32 fontsCount = array_size(fontNames);
static_assert(array_size(fontNames) == array_size(labelTexts));

constexpr Real fontSizes[] = { 9, 11, 14, 18 };

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		Holder g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->panel();
		auto _2 = g->scrollbars();
		auto _3 = g->alignment();
		auto _4 = g->verticalTable(array_size(fontSizes) + 1);
		{
			g->label().text("");
			for (Real size : fontSizes)
				g->label().text(Stringizer() + size);
		}
		for (uint32 fontIndex = 0; fontIndex < fontsCount; fontIndex++)
		{
			g->label().text(labelTexts[fontIndex]);
			for (Real size : fontSizes)
			{
				GuiTextFormatComponent f;
				f.font = HashString(fontNames[fontIndex]);
				f.size = size;
				g->label().text(multilineText).textFormat(f);
			}
		}
	}
};

MAIN(GuiTestImpl, "fonts sizes")
