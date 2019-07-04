#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		uint32 an = 0;
		{
			entity *e = ents->createUnique();
			an = e->name();
			CAGE_COMPONENT_GUI(scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 bn = 0;
		{
			entity *e = ents->createUnique();
			bn = e->name();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = an;
			CAGE_COMPONENT_GUI(panel, panel, e);
			CAGE_COMPONENT_GUI(text, text, e);
			text.value = "Outer";
			CAGE_COMPONENT_GUI(explicitSize, size, e);
			size.size = vec2(400, 400);
			CAGE_COMPONENT_GUI(scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 cn = 0;
		{
			entity *e = ents->createUnique();
			cn = e->name();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = bn;
			CAGE_COMPONENT_GUI(spoiler, spoiler, e);
			CAGE_COMPONENT_GUI(text, text, e);
			text.value = "Inner";
		}

		{
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = cn;
			CAGE_COMPONENT_GUI(label, lab, e);
			CAGE_COMPONENT_GUI(image, img, e);
			img.textureName = hashString("cage/texture/helper.jpg");
			CAGE_COMPONENT_GUI(explicitSize, size, e);
			size.size = vec2(500, 500);
		}
	}

};

MAIN(guiTestImpl, "scrollbars")
