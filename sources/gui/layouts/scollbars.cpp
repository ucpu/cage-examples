#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		uint32 an = 0;
		{
			Entity *e = ents->createUnique();
			an = e->name();
			GuiScrollbarsComponent &sc = e->value<GuiScrollbarsComponent>();
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 bn = 0;
		{
			Entity *e = ents->createUnique();
			bn = e->name();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = an;
			GuiPanelComponent &panel = e->value<GuiPanelComponent>();
			GuiTextComponent &text = e->value<GuiTextComponent>();
			text.value = "Outer";
			GuiExplicitSizeComponent &size = e->value<GuiExplicitSizeComponent>();
			size.size = vec2(400, 400);
			GuiScrollbarsComponent &sc = e->value<GuiScrollbarsComponent>();
			sc.alignment = vec2(0.5, 0.5);
		}

		uint32 cn = 0;
		{
			Entity *e = ents->createUnique();
			cn = e->name();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = bn;
			GuiSpoilerComponent &spoiler = e->value<GuiSpoilerComponent>();
			GuiTextComponent &text = e->value<GuiTextComponent>();
			text.value = "Inner";
		}

		{
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = cn;
			GuiLabelComponent &lab = e->value<GuiLabelComponent>();
			GuiImageComponent &img = e->value<GuiImageComponent>();
			img.textureName = HashString("cage/texture/helper.jpg");
			GuiExplicitSizeComponent &size = e->value<GuiExplicitSizeComponent>();
			size.size = vec2(500, 500);
		}
	}

};

MAIN(guiTestImpl, "scrollbars")
