#include "../gui.h"

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
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "text";
		}

		{ // with explicit size
			guiLabel(3, index, "with explicit size");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "text";
			e->value<GuiExplicitSizeComponent>().size = Vec2(0, 100);
		}

		{ // with tooltip
			guiLabel(3, index, "with tooltip");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "text";
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"press me">();
			e->value<GuiTooltipComponent>().delay = 1000;
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
				p.parent = layout->name();
				p.order = index++;
				e->value<GuiButtonComponent>();
				e->value<GuiTextComponent>().value = Stringizer() + i;
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

		{ // with unloaded image
			guiLabel(3, index, "with unloaded image");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiImageComponent>().textureName = HashString("cage-tests/decals/albedo.png");
		}

		/*
		{ // with invalid image
			guiLabel(3, index, "with invalid image");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiImageComponent>().textureName = HashString("cage/texture/helper.jpg___");
		}
		*/

		{ // with custom event
			guiLabel(3, index, "with custom event");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "custom event";
			e->value<GuiEventComponent>().event.bind<GuiTestImpl, &GuiTestImpl::onButtonPressed>(this);
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "text";
			e->value<GuiWidgetStateComponent>().disabled = true;
		}
	}

	bool onButtonPressed(Entity *)
	{
		CAGE_LOG(SeverityEnum::Info, "event", "button with custom event pressed");
		return false;
	}
};

MAIN(GuiTestImpl, "buttons")
