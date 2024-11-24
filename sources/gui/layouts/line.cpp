#include "../gui.h"

namespace cage
{
	namespace detail
	{
		template<uint32 N>
		CAGE_FORCE_INLINE StringizerBase<N> &operator+(StringizerBase<N> &str, LineEdgeModeEnum other)
		{
			switch (other)
			{
				case LineEdgeModeEnum::None:
					return str + "none";
				case LineEdgeModeEnum::Flexible:
					return str + "flexible";
				case LineEdgeModeEnum::Spaced:
					return str + "spaced";
				case LineEdgeModeEnum::Empty:
					return str + "empty";
				default:
					CAGE_THROW_CRITICAL(Exception, "invalid LineEdgeModeEnum");
			}
		}
	}
}

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	void generate(Holder<GuiBuilder> &g, uint32 count = 5)
	{
		for (uint32 i = 0; i < count; i++)
		{
			auto _1 = g->panel().text(Stringizer() + i).skin(GuiSkinTooltips);
			auto _2 = g->column();
			const uint32 cnt = randomRange(0, 3);
			for (uint32 j = 0; j < cnt; j++)
				g->label().text(randomChance() < 0.5 ? "hello" : "hi");
		}
	}

	void initialize() override
	{
		Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities());
		auto _1 = g->horizontalScrollbar(true);
		auto _2 = g->row(0.5);

		{
			auto _1 = g->panel().text("basic");
			auto _2 = g->verticalScrollbar();
			auto _3 = g->topColumn();
			{
				auto _1 = g->panel().text("row");
				auto _2 = g->row();
				generate(g);
			}
			{
				auto _1 = g->panel().text("row spaced");
				auto _2 = g->spacedRow();
				generate(g);
			}
			{
				auto _1 = g->panel().text("left row");
				auto _2 = g->leftRow();
				generate(g);
			}
			{
				auto _1 = g->panel().text("left row flexible right");
				auto _2 = g->leftRowStretchRight();
				generate(g);
			}
			{
				auto _1 = g->panel().text("right row");
				auto _2 = g->rightRow();
				generate(g);
			}
			{
				auto _1 = g->panel().text("right row flexible left");
				auto _2 = g->rightRowStretchLeft();
				generate(g);
			}
			{
				auto _1 = g->panel().text("center row");
				auto _2 = g->centerRow();
				generate(g);
			}
			{
				auto _1 = g->panel().text("center row flexible edges");
				auto _2 = g->centerRowStretchBoth();
				generate(g);
			}
			{
				auto _1 = g->panel().text("row vertalign 0");
				auto _2 = g->row(0);
				generate(g);
			}
			{
				auto _1 = g->panel().text("row vertalign 0.5");
				auto _2 = g->row(0.5);
				generate(g);
			}
			{
				auto _1 = g->panel().text("row vertalign 1");
				auto _2 = g->row(1);
				generate(g);
			}
			g->panel().text("extra width").size(Vec2(400, 50));
		}
		{
			auto _1 = g->panel().text("fixed width (varying counts, no scrolls)").size(Vec2(300, Real::Nan()));
			auto _2 = g->verticalScrollbar();
			auto _3 = g->topColumn();
			{
				auto _1 = g->panel().text("1");
				auto _2 = g->row();
				generate(g, 1);
			}
			{
				auto _1 = g->panel().text("3");
				auto _2 = g->row();
				generate(g, 3);
			}
			{
				auto _1 = g->panel().text("10");
				auto _2 = g->row();
				generate(g, 10);
			}
			{
				auto _1 = g->panel().text("20");
				auto _2 = g->row();
				generate(g, 20);
			}
		}

		{
			auto _1 = g->panel().text("fixed width (20 count, no scrolls)").size(Vec2(300, Real::Nan()));
			auto _2 = g->verticalScrollbar();
			auto _3 = g->topColumn();
			{
				auto _1 = g->panel().text("row");
				auto _2 = g->row();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("row spaced");
				auto _2 = g->spacedRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("left row");
				auto _2 = g->leftRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("left row flexible right");
				auto _2 = g->leftRowStretchRight();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("right row");
				auto _2 = g->rightRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("right row flexible left");
				auto _2 = g->rightRowStretchLeft();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("center row");
				auto _2 = g->centerRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("center row flexible edges");
				auto _2 = g->centerRowStretchBoth();
				generate(g, 20);
			}
		}

		{
			auto _1 = g->panel().text("fixed width (20 count, with scrolls)").size(Vec2(300, Real::Nan()));
			auto _2 = g->verticalScrollbar();
			auto _3 = g->topColumn();
			{
				auto _1 = g->panel().text("row");
				auto _2 = g->scrollbars();
				auto _3 = g->row();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("row spaced");
				auto _2 = g->scrollbars();
				auto _3 = g->spacedRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("left row");
				auto _2 = g->scrollbars();
				auto _3 = g->leftRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("left row flexible right");
				auto _2 = g->scrollbars();
				auto _3 = g->leftRowStretchRight();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("right row");
				auto _2 = g->scrollbars();
				auto _3 = g->rightRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("right row flexible left");
				auto _2 = g->scrollbars();
				auto _3 = g->rightRowStretchLeft();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("center row");
				auto _2 = g->scrollbars();
				auto _3 = g->centerRow();
				generate(g, 20);
			}
			{
				auto _1 = g->panel().text("center row flexible edges");
				auto _2 = g->scrollbars();
				auto _3 = g->centerRowStretchBoth();
				generate(g, 20);
			}
		}

		for (uint32 count = 0; count < 4; count++)
		{
			auto _1 = g->panel().text(Stringizer() + "count: " + count);
			auto _2 = g->verticalScrollbar();
			auto _3 = g->topColumn();
			for (LineEdgeModeEnum l : { LineEdgeModeEnum::None, LineEdgeModeEnum::Flexible, LineEdgeModeEnum::Empty })
			{
				for (LineEdgeModeEnum r : { LineEdgeModeEnum::None, LineEdgeModeEnum::Flexible, LineEdgeModeEnum::Empty })
				{
					auto _1 = g->panel().text(Stringizer() + l + "-" + r);
					auto _2 = g->row();
					_2->value<GuiLayoutLineComponent>().first = l;
					_2->value<GuiLayoutLineComponent>().last = r;
					generate(g, count);
				}
			}
			g->panel().text("extra width").size(Vec2((count + 1) * 100, 50));
		}

		{
			auto _1 = g->panel().text("flexible is the one that shrinks (1)");
			auto _2 = g->verticalScrollbar();
			auto _3 = g->column();
			{
				auto _1 = g->panel().text("left row");
				auto _2 = g->column(0);
				for (Real w = 100; w < 300; w += 60)
				{
					auto _1 = g->panel().text(Stringizer() + w).size(Vec2(w, Real::Nan()));
					auto _2 = g->leftRowStretchRight();
					g->panel().skin(GuiSkinTooltips).text("fixed left");
					g->panel().skin(GuiSkinTooltips).text("flexible right");
				}
			}
			{
				auto _1 = g->panel().text("center row");
				auto _2 = g->column(0);
				for (Real w = 100; w < 300; w += 60)
				{
					auto _1 = g->panel().text(Stringizer() + w).size(Vec2(w, Real::Nan()));
					auto _2 = g->centerRowStretchBoth();
					g->panel().skin(GuiSkinTooltips).text("flexible left");
					g->panel().skin(GuiSkinTooltips).text("flexible right");
				}
			}
			{
				auto _1 = g->panel().text("right row");
				auto _2 = g->column(0);
				for (Real w = 100; w < 300; w += 60)
				{
					auto _1 = g->panel().text(Stringizer() + w).size(Vec2(w, Real::Nan()));
					auto _2 = g->rightRowStretchLeft();
					g->panel().skin(GuiSkinTooltips).text("flexible left");
					g->panel().skin(GuiSkinTooltips).text("fixed right");
				}
			}
		}

		{
			auto _1 = g->panel().text("flexible is the one that shrinks (2)");
			auto _2 = g->verticalScrollbar();
			auto _3 = g->column();
			{
				auto _1 = g->panel().text("left row").size(Vec2(200, Real::Nan()));
				auto _2 = g->column(0);
				for (uint32 cnt = 0; cnt < 10; cnt += 3)
				{
					auto _1 = g->panel().text(Stringizer() + cnt);
					auto _2 = g->leftRowStretchRight();
					{
						auto _ = g->panel().skin(GuiSkinTooltips).text("fixed left");
						g->label().text("hej");
					}
					generate(g, cnt);
				}
			}
		}
	}
};

MAIN(GuiTestImpl, "line")
