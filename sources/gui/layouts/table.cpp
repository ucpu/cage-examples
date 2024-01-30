#include "../gui.h"

#include <cage-core/string.h>

using namespace cage;

bool setGrid(input::GuiValue in)
{
	engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().grid = in.entity->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked;
	return true;
}

bool setVertical(input::GuiValue in)
{
	engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().vertical = in.entity->value<GuiCheckBoxComponent>().state == CheckBoxStateEnum::Checked;
	return true;
}

bool setSegments(input::GuiValue in)
{
	if (in.entity->value<GuiInputComponent>().valid)
		engineGuiEntities()->get(42)->value<GuiLayoutTableComponent>().sections = toUint32(in.entity->value<GuiInputComponent>().value);
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
			g->checkBox().text("grid").event(inputFilter(setGrid));
			g->checkBox(true).text("vertical").event(inputFilter(setVertical));
			g->input(2, 0, 10).text("sections").event(inputFilter(setSegments));
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
