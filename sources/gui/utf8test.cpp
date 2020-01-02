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
		EntityManager *ents = engineGui()->entities();

		Entity *panel = ents->create(2);
		{
			CAGE_COMPONENT_GUI(Panel, gp, panel);
			CAGE_COMPONENT_GUI(LayoutLine, ll, panel);
			ll.vertical = true;
			CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}

		for (uint32 i = 0; i < 300; i++)
		{
			Entity * e = ents->create(100 + i);
			CAGE_COMPONENT_GUI(Parent, parent, e);
			parent.parent = panel->name();
			parent.order = i;
			CAGE_COMPONENT_GUI(Label, label, e);
			CAGE_COMPONENT_GUI(Text, text, e);
			text.assetName = HashString("cage-tests/gui/utf8test.textpack");
			text.textName = HashString(string(stringizer() + "utf8test/" + i));
			CAGE_COMPONENT_GUI(TextFormat, format, e);
			format.font = HashString("cage-tests/gui/DroidSansMono.ttf");
		}
	}

};

MAIN(guiTestImpl, "utf8 test")
