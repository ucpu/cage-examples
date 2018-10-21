#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		uint32 an = 0;
		{
			entityClass *e = ents->createUnique();
			an = e->name();
			GUI_GET_COMPONENT(scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 bn = 0;
		{
			entityClass *e = ents->createUnique();
			bn = e->name();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = an;
			GUI_GET_COMPONENT(panel, panel, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "Outer";
			GUI_GET_COMPONENT(explicitSize, size, e);
			size.size = vec2(400, 400);
			GUI_GET_COMPONENT(scrollbars, sc, e);
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 cn = 0;
		{
			entityClass *e = ents->createUnique();
			cn = e->name();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = bn;
			GUI_GET_COMPONENT(spoiler, spoiler, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "Inner";
		}

		{
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = cn;
			GUI_GET_COMPONENT(label, lab, e);
			GUI_GET_COMPONENT(image, img, e);
			img.textureName = hashString("cage/texture/helper.jpg");
			GUI_GET_COMPONENT(explicitSize, size, e);
			size.size = vec2(500, 500);
		}
	}

};

MAIN(guiTestImpl, "scrollbars")
