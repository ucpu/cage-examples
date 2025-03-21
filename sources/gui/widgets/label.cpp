#include "../gui.h"

using namespace cage;

class GuiTestImpl : public GuiTestClass
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
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "text";
		}

		{ // with simple tooltip
			guiLabel(3, index, "with simple tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "hover over this";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "How are you doing?">();
		}

		{ // with asset tooltip
			guiLabel(3, index, "with asset tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "hover over this";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<HashString("texts/hello")>();
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
				p.parent = layout->id();
				p.order = index++;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = Stringizer() + i;
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
				p.parent = layout->id();
				p.order = index++;
				e->value<GuiLabelComponent>();
				e->value<GuiTextComponent>().value = Stringizer() + i;
			}
		}

		{ // with image
			guiLabel(3, index, "with image");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			GuiImageComponent &m = e->value<GuiImageComponent>();
			m.textureName = HashString("cage/textures/helper.jpg");
			m.textureUvOffset = Vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = Vec2(1 / 8.f, 1 / 8.f);
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<0, "helper.jpg">();
		}
	}
};

MAIN(GuiTestImpl, "labels")
