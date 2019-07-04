#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // with text
			guiLabel(3, index, "with text");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(button, b, e);
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "text";
		}
		{ // horizontal
			guiLabel(3, index, "horizontal");
			entity *layout = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, layout);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(layoutLine, l, layout);
			for (uint32 i = 0; i < 4; i++)
			{
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(button, b, e);
				//b.allowMerging = true;
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = i;
			}
		}
		{ // vertical
			guiLabel(3, index, "vertical");
			entity *layout = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, layout);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(layoutLine, l, layout);
			l.vertical = true;
			for (uint32 i = 0; i < 4; i++)
			{
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(button, b, e);
				//b.allowMerging = true;
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = i;
			}
		}
		{ // with image
			guiLabel(3, index, "with image");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(button, b, e);
			CAGE_COMPONENT_GUI(image, m, e);
			m.textureName = hashString("cage/texture/helper.jpg");
			m.textureUvOffset = vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = vec2(1 / 8.f, 1 / 8.f);
		}
	}

};

MAIN(guiTestImpl, "buttons")
