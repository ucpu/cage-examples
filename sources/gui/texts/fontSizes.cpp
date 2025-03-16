#include "../gui.h"

using namespace cage;

constexpr const char *FontNames[] = {
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
constexpr String LabelTexts[] = {
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
constexpr String multilineText = "lorem ipsum dolor sit amet\npříliš žluťoučký kůň úpěl ďábelské ódy\na quick brown fox jumps over a lazy dog\nhello world, foo, bar, baz\n\n";

const uint32 fontsCount = array_size(FontNames);
static_assert(array_size(FontNames) == array_size(LabelTexts));

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
			g->label().text(LabelTexts[fontIndex]);
			for (Real size : fontSizes)
			{
				GuiTextFormatComponent f;
				f.font = HashString(FontNames[fontIndex]);
				f.size = size;
				g->label().text(multilineText).textFormat(f);
			}
		}
	}
};

MAIN(GuiTestImpl, "fonts sizes")
