#include "../gui.h"

#include <cage-core/lineReader.h>

using namespace cage;

PointerRange<const uint8> utf8stressTestTxt();

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		auto g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->panel();
		auto _2 = g->scrollbars();
		auto _3 = g->alignment();
		auto _4 = g->column();
		const auto lr = newLineReader(utf8stressTestTxt().cast<const char>());
		String s;
		while (lr->readLine(s))
			g->label().text(s).textFormat(GuiTextFormatComponent{ .font = HashString("cage/fonts/ubuntu/monospace.ttf") });
	}
};

MAIN(GuiTestImpl, "utf8 stress test")
