#include "../gui.h"

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
		auto _ = g->horizontalTable(3, true);
		for (uint32 y = 0; y < 3; y++)
		{
			for (uint32 x = 0; x < 3; x++)
			{
				const Vec2 a = Vec2(x, y) * 0.5;
				auto _1 = g->panel();
				auto _2 = g->scrollbars();
				auto _3 = g->alignment(a);
				auto _4 = g->panel().text(Stringizer() + "x: " + x + ", y: " + y);
				g->label().text(Stringizer() + a).textSize(200);
			}
		}
	}
};

MAIN(GuiTestImpl, "scrollbars")
