#include "gui.h"

// *************************************************************************
// This process may fail due to the text containing malformed utf-8 text.
// It is NOT AN ISSUE if the process throws "invalid utf8 string" exception
// in debug build.
// *************************************************************************

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		Entity *panel = ents->create(2);
		{
			panel->value<GuiPanelComponent>();
			GuiLayoutLineComponent &ll = panel->value<GuiLayoutLineComponent>();
			ll.vertical = true;
			GuiScrollbarsComponent &sc = panel->value<GuiScrollbarsComponent>();
			sc.alignment = Vec2(0.5, 0);
		}

		for (uint32 i = 0; i < 300; i++)
		{
			Entity * e = ents->create(100 + i);
			GuiParentComponent &parent = e->value<GuiParentComponent>();
			parent.parent = panel->name();
			parent.order = i;
			e->value<GuiLabelComponent>();
			GuiTextComponent &text = e->value<GuiTextComponent>();
			text.assetName = HashString("cage-tests/gui/utf8test.textpack");
			text.textName = HashString(String(Stringizer() + "utf8test/" + i));
			GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
			format.font = HashString("cage-tests/gui/DroidSansMono.ttf");
		}
	}

};

MAIN(guiTestImpl, "utf8 test")
