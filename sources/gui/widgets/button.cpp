#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // with text
			guiLabel(3, index, "with text");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "text";
		}
		{ // horizontal
			guiLabel(3, index, "horizontal");
			Entity *layout = ents->createUnique();
			GuiParentComponent &p = layout->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			layout->value<GuiLayoutLineComponent>();
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = layout->name();
				p.order = index++;
				e->value<GuiButtonComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + i;
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
				e->value<GuiButtonComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + i;
			}
		}
		{ // with image
			guiLabel(3, index, "with image");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			GuiImageComponent &m = e->value<GuiImageComponent>();
			m.textureName = HashString("cage/texture/helper.jpg");
			m.textureUvOffset = Vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = Vec2(1 / 8.f, 1 / 8.f);
		}
		{ // with custom event
			guiLabel(3, index, "with custom event");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
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
