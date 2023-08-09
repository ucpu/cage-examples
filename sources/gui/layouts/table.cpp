#include "../gui.h"

#include <cage-core/string.h>

using namespace cage;

bool setGrid(Entity *c)
{
	engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().grid = c->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked;
	return true;
}

bool setVertical(Entity *c)
{
	engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().vertical = c->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked;
	return true;
}

bool setSegments(Entity *c)
{
	if (c->value<GuiInputComponent>().valid)
		engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().sections = toUint32(c->value<GuiInputComponent>().value);
	return true;
}

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		guiBasicLayout();

		{ // controls
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(2));
			auto _ = g->leftRow();
			g->checkBox().text("grid").bind<&setGrid>();
			g->checkBox(true).text("vertical").bind<&setVertical>();
			g->input(2, 0, 10).text("sections").bind<&setSegments>();
		}

		{ // the table
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(3));
			auto _ = g->setNextName(42).verticalTable();
			for (uint32 i = 0; i < 15; i++)
			{
				auto _1 = g->panel().text(Stringizer() + i);
				auto _2 = g->middleColumn();
				const uint32 cnt = randomRange(0, 3);
				for (uint32 j = 0; j < cnt; j++)
					g->label().text(fill(String("Hello"), randomRange(0, 10), '_'));
			}
		}
	}
};

MAIN(GuiTestImpl, "table")
