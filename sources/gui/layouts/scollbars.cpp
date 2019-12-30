#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		EntityManager *ents = gui()->entities();

		uint32 an = 0;
		{
			Entity *e = ents->createUnique();
			an = e->name();
			CAGE_COMPONENT_GUI(Scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 bn = 0;
		{
			Entity *e = ents->createUnique();
			bn = e->name();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = an;
			CAGE_COMPONENT_GUI(Panel, panel, e);
			CAGE_COMPONENT_GUI(Text, text, e);
			text.value = "Outer";
			CAGE_COMPONENT_GUI(ExplicitSize, size, e);
			size.size = vec2(400, 400);
			CAGE_COMPONENT_GUI(Scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 cn = 0;
		{
			Entity *e = ents->createUnique();
			cn = e->name();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = bn;
			CAGE_COMPONENT_GUI(Spoiler, spoiler, e);
			CAGE_COMPONENT_GUI(Text, text, e);
			text.value = "Inner";
		}

		{
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = cn;
			CAGE_COMPONENT_GUI(Label, lab, e);
			CAGE_COMPONENT_GUI(Image, img, e);
			img.textureName = HashString("cage/texture/helper.jpg");
			CAGE_COMPONENT_GUI(ExplicitSize, size, e);
			size.size = vec2(500, 500);
		}
	}

};

MAIN(guiTestImpl, "scrollbars")
