#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		guiBasicLayout();
		{
			entityClass *e = ents->get(3);
			GUI_GET_COMPONENT(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // with text
			guiLabel(3, index, "with text");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(button, b, e);
			GUI_GET_COMPONENT(text, t, e);
			t.value = "text";
		}
		{ // horizontal
			guiLabel(3, index, "horizontal");
			entityClass *layout = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, layout);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(layoutLine, l, layout);
			for (uint32 i = 0; i < 4; i++)
			{
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				GUI_GET_COMPONENT(button, b, e);
				b.allowMerging = true;
				GUI_GET_COMPONENT(text, t, e);
				t.value = i;
			}
		}
		{ // vertical
			guiLabel(3, index, "vertical");
			entityClass *layout = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, layout);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(layoutLine, l, layout);
			l.vertical = true;
			for (uint32 i = 0; i < 4; i++)
			{
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				GUI_GET_COMPONENT(button, b, e);
				b.allowMerging = true;
				GUI_GET_COMPONENT(text, t, e);
				t.value = i;
			}
		}
		{ // with image
			guiLabel(3, index, "with image");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(button, b, e);
			GUI_GET_COMPONENT(image, m, e);
			m.textureName = hashString("cage/texture/helper.jpg");
			m.textureUvOffset = vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = vec2(1 / 8.f, 1 / 8.f);
		}
	}

};

MAIN(guiTestImpl, "buttons")
