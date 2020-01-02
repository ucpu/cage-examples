#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // with text
			guiLabel(3, index, "with text");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Button, b, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "text";
		}
		{ // horizontal
			guiLabel(3, index, "horizontal");
			Entity *layout = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, layout);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(LayoutLine, l, layout);
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(Button, b, e);
				//b.allowMerging = true;
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(i);
			}
		}
		{ // vertical
			guiLabel(3, index, "vertical");
			Entity *layout = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, layout);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(LayoutLine, l, layout);
			l.vertical = true;
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = layout->name();
				p.order = index++;
				CAGE_COMPONENT_GUI(Button, b, e);
				//b.allowMerging = true;
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(i);
			}
		}
		{ // with image
			guiLabel(3, index, "with image");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Button, b, e);
			CAGE_COMPONENT_GUI(Image, m, e);
			m.textureName = HashString("cage/texture/helper.jpg");
			m.textureUvOffset = vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = vec2(1 / 8.f, 1 / 8.f);
		}
		{ // with custom event
			guiLabel(3, index, "with custom event");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Button, b, e);
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "custom event";
			CAGE_COMPONENT_GUI(Event, ev, e);
			ev.event.bind<guiTestImpl, &guiTestImpl::onButtonPressed>(this);
		}
	}

	bool onButtonPressed(uint32 en)
	{
		CAGE_LOG(SeverityEnum::Info, "event", "button with custom event pressed");
		return false;
	}

};

MAIN(guiTestImpl, "buttons")
