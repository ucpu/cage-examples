#include "../gui.h"

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	void generate(Holder<GuiBuilder> &g, uint32 count)
	{
		for (uint32 i = 0; i < count; i++)
		{
			auto _1 = g->panel().text(Stringizer() + i).skin(3);
			auto _2 = g->column();
			const uint32 cnt = randomRange(0, 3);
			for (uint32 j = 0; j < cnt; j++)
				g->label().text(randomChance() < 0.5 ? "hello" : "hi");
		}
	}

	void initialize() override
	{
		Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->scrollbars();
		auto _2 = g->row(false, 0.5);

		{
			auto _1 = g->panel().text("horizontal split 2");
			auto _2 = g->horizontalSplit();
			generate(g, 2);
		}
		{
			auto _1 = g->panel().text("horizontal split 3");
			auto _2 = g->horizontalSplit();
			generate(g, 3);
		}
		{
			auto _1 = g->panel().text("vertical split 0");
			auto _2 = g->verticalSplit();
		}
		{
			auto _1 = g->panel().text("vertical split 1");
			auto _2 = g->verticalSplit();
			generate(g, 1);
		}
		{
			auto _1 = g->panel().text("vertical split 2");
			auto _2 = g->verticalSplit();
			generate(g, 2);
		}
		{
			auto _1 = g->panel().text("vertical split 5");
			auto _2 = g->verticalSplit();
			generate(g, 5);
		}
		{
			auto _1 = g->panel().text("vertical split left align");
			auto _2 = g->verticalSplit(0);
			generate(g, 5);
		}
		{
			auto _1 = g->panel().text("vertical split center align");
			auto _2 = g->verticalSplit(0.5);
			generate(g, 5);
		}
		{
			auto _1 = g->panel().text("vertical split right align");
			auto _2 = g->verticalSplit(1);
			generate(g, 5);
		}
	}
};

MAIN(GuiTestImpl, "line")
