#include "../gui.h"

#include <cage-core/lineReader.h>
#include <cage-core/unicode.h>

using namespace cage;

PointerRange<const uint8> multilingualQuotesTxt();

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		detail::GuiTextFontDefault = HashString("cage-tests/gui/noto/universal.ttf");
		auto g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->panel();
		auto _2 = g->scrollbars();
		auto _3 = g->alignment();
		auto _4 = g->column();
		const auto lr = newLineReader(multilingualQuotesTxt().cast<const char>());
		String s;
		while (lr->readLine(s))
		{
			g->label().text(s);
			/*
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::CanonicalComposition }));
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::CanonicalDecomposition }));
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::Casefold }));
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::Lowercase }));
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::Titlecase }));
			g->label().text(unicodeTransformString(s, UnicodeTransformConfig{ UnicodeTransformEnum::Uppercase }));
			*/
		}
	}
};

MAIN(GuiTestImpl, "multilingual quotes")
