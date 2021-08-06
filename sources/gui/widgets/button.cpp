#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			GuiLayoutTableComponent &layout = e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // with text
			guiLabel(3, index, "with text");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiButtonComponent &b = e->value<GuiButtonComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "text";
		}
		{ // horizontal
			guiLabel(3, index, "horizontal");
			Entity *layout = ents->createUnique();
			GuiParentComponent &p = layout->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiLayoutLineComponent &l = layout->value<GuiLayoutLineComponent>();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = layout->name();
				p.order = index++;
				GuiButtonComponent &b = e->value<GuiButtonComponent>();
				//b.allowMerging = true;
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = stringizer() + i;
			}
		}
		{ // vertical
			guiLabel(3, index, "vertical");
			Entity *layout = ents->createUnique();
			GuiParentComponent &p = layout->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiLayoutLineComponent &l = layout->value<GuiLayoutLineComponent>();
			l.vertical = true;
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = layout->name();
				p.order = index++;
				GuiButtonComponent &b = e->value<GuiButtonComponent>();
				//b.allowMerging = true;
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = stringizer() + i;
			}
		}
		{ // with image
			guiLabel(3, index, "with image");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiButtonComponent &b = e->value<GuiButtonComponent>();
			GuiImageComponent &m = e->value<GuiImageComponent>();
			m.textureName = HashString("cage/texture/helper.jpg");
			m.textureUvOffset = vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = vec2(1 / 8.f, 1 / 8.f);
		}
		{ // with custom event
			guiLabel(3, index, "with custom event");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiButtonComponent &b = e->value<GuiButtonComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "custom event";
			GuiEventComponent &ev = e->value<GuiEventComponent>();
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
