#include "gui.h"

// *************************************************************************
// this processHandle may fail due to the text containing malformed utf-8 text
// it is NOT AN ISSUE if the processHandle throws "invalid utf8 string" exception
// in debug build
// *************************************************************************

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		entity *panel = ents->create(2);
		{
			CAGE_COMPONENT_GUI(panel, gp, panel);
			CAGE_COMPONENT_GUI(layoutLine, ll, panel);
			ll.vertical = true;
			CAGE_COMPONENT_GUI(scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}

		for (uint32 i = 0; i < 300; i++)
		{
			entity * e = ents->create(100 + i);
			CAGE_COMPONENT_GUI(parent, parent, e);
			parent.parent = panel->name();
			parent.order = i;
			CAGE_COMPONENT_GUI(label, label, e);
			CAGE_COMPONENT_GUI(text, text, e);
			text.assetName = hashString("cage-tests/gui/utf8test.textpack");
			text.textName = hashString(string(stringizer() + "utf8test/" + i));
			CAGE_COMPONENT_GUI(textFormat, format, e);
			format.font = hashString("cage-tests/gui/DroidSansMono.ttf");
		}
	}

};

MAIN(guiTestImpl, "utf8 test")
