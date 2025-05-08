#include "../gui.h"

#include <cage-engine/guiBuilder.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	void makeTexts(GuiBuilder *g)
	{
		g->label().text("May the Force be with you. _");
		g->label().text("I have a bad feeling about this. _");
		g->label().text("No, I am your father. _");
		g->label().text("Do. Or do not. There is no try. _");
		g->label().text("It’s a trap! _");
		g->label().text("I’m one with the Force. The Force is with me. _");
		g->label().text("Rebellions are built on hope. _");
		g->label().text("I find your lack of faith disturbing. _");
		g->label().text("These aren’t the droids you’re looking for. _");
		g->label().text("Help me, Obi-Wan Kenobi. You’re my only hope. _");
		g->label().text("Wars not make one great. _");
		g->label().text("It’s over, Anakin. I have the high ground. _");
		g->label().text("Never tell me the odds! _");
		g->label().text("Why, you stuck-up, half-witted, scruffy-looking …nerf-herder! _");
		g->label().text("I don’t like sand. It’s coarse and rough and irritating, and it gets everywhere. _");
		g->label().text("This is how liberty dies … with thunderous applause. _");
		g->label().text("That’s no moon. _");
		g->label().text("Chewie, we’re home. _");
	}

	void initialize() override
	{
		guiBasicLayout();
		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(2));
			auto _ = g->leftRow();
			makeTexts(+g);
		}
		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(3));
			auto _ = g->leftRow();
			for (Real w = 100; w < 250; w += 30)
			{
				auto _ = g->topColumn().size(Vec2(w, Real::Nan()));
				g->label().text(Stringizer() + w);
				makeTexts(+g);
			}
			for (Real w = 100; w < 250; w += 30)
			{
				auto _1 = g->panel().size(Vec2(w, Real::Nan())).text(Stringizer() + w);
				auto _2 = g->topColumn();
				makeTexts(+g);
			}
		}
	}
};

MAIN(GuiTestImpl, "text layout")
