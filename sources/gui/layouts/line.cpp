#include "../gui.h"

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	void generate(Holder<GuiBuilder> &g, uint32 count = 5)
	{
		for (uint32 i = 0; i < count; i++)
		{
			auto _1 = g->panel().text(Stringizer() + i);
			auto _2 = g->column();
			const uint32 cnt = randomRange(0, 3);
			for (uint32 j = 0; j < cnt; j++)
				g->label().text("hello");
		}
	}

	void initialize() override
	{
		Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->scrollbars();
		auto _2 = g->column();

		{
			auto _1 = g->panel().text("row");
			auto _2 = g->row();
			generate(g);
		}

		{
			auto _1 = g->panel().text("row spaced");
			auto _2 = g->row(true);
			generate(g);
		}

		{
			auto _1 = g->panel().text("left row");
			auto _2 = g->leftRow();
			generate(g);
		}

		{
			auto _1 = g->panel().text("left row flexible right");
			auto _2 = g->leftRow(true);
			generate(g);
		}

		{
			auto _1 = g->panel().text("right row");
			auto _2 = g->rightRow();
			generate(g);
		}

		{
			auto _1 = g->panel().text("right row flexible left");
			auto _2 = g->rightRow(true);
			generate(g);
		}

		{
			auto _1 = g->panel().text("center row");
			auto _2 = g->centerRow();
			generate(g);
		}

		{
			auto _1 = g->panel().text("center row flexible edges");
			auto _2 = g->centerRow(true);
			generate(g);
		}

		{
			auto _1 = g->panel().text("row crossalign 0");
			auto _2 = g->row(false, 0);
			generate(g);
		}

		{
			auto _1 = g->panel().text("row crossalign 0.5");
			auto _2 = g->row(false, 0.5);
			generate(g);
		}

		{
			auto _1 = g->panel().text("row crossalign 1");
			auto _2 = g->row(false, 1);
			generate(g);
		}

		for (uint32 count = 0; count < 3; count++)
		{
			{
				auto _1 = g->panel().text(Stringizer() + "center row, count: " + count);
				auto _2 = g->centerRow();
				generate(g, count);
			}
			{
				auto _1 = g->panel().text(Stringizer() + "center row flexible edges, count: " + count);
				auto _2 = g->centerRow(true);
				generate(g, count);
			}
			{
				auto _1 = g->panel().text(Stringizer() + "empty-flexible, count: " + count);
				auto _2 = g->row();
				_2->value<GuiLayoutLineComponent>().begin = LineEdgeModeEnum::Empty;
				_2->value<GuiLayoutLineComponent>().end = LineEdgeModeEnum::Flexible;
				generate(g, count);
			}
			{
				auto _1 = g->panel().text(Stringizer() + "flexible-empty, count: " + count);
				auto _2 = g->row();
				_2->value<GuiLayoutLineComponent>().begin = LineEdgeModeEnum::Flexible;
				_2->value<GuiLayoutLineComponent>().end = LineEdgeModeEnum::Empty;
				generate(g, count);
			}
		}
	}
};

MAIN(GuiTestImpl, "line")
