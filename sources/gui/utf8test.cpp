#include "gui.h"

// *************************************************************************
// this program may fail due to the text containing malformed utf-8 text
// it is NOT AN ISSUE if the program throws "invalid utf8 string" exception
// in debug build
// *************************************************************************

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		entityClass *panel = ents->create(2);
		{
			GUI_GET_COMPONENT(panel, gp, panel);
			GUI_GET_COMPONENT(position, pos, panel);
			pos.size.units[0] = unitEnum::ScreenWidth;
			pos.size.units[1] = unitEnum::ScreenHeight;
			pos.size.value = vec2(1, 1);
			GUI_GET_COMPONENT(layoutLine, ll, panel);
			ll.vertical = true;
		}

		for (uint32 i = 0; i < 300; i++)
		{
			entityClass * e = ents->create(100 + i);
			GUI_GET_COMPONENT(parent, parent, e);
			parent.parent = panel->name();
			parent.order = i;
			GUI_GET_COMPONENT(label, label, e);
			GUI_GET_COMPONENT(text, text, e);
			text.assetName = hashString("cage-tests/gui/utf8test.textpack");
			text.textName = hashString((string() + "utf8test/" + i).c_str());
			GUI_GET_COMPONENT(textFormat, format, e);
			format.font = hashString("cage-tests/gui/DroidSansMono.ttf");
		}
	}

};

MAIN(guiTestImpl, "splitter")
