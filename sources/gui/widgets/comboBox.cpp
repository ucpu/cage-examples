#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			GuiLayoutTableComponent &layout = e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // empty
			guiLabel(3, index, "empty");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiComboBoxComponent &cb = e->value<GuiComboBoxComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "placeholder";
		}
		{ // items
			guiLabel(3, index, "items");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiComboBoxComponent &cb = e->value<GuiComboBoxComponent>();
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				GuiTextComponent &t = o->value<GuiTextComponent>();
				t.value = Stringizer() + "option " + i;
			}
		}
		{ // preselected
			guiLabel(3, index, "preselected");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiComboBoxComponent &cb = e->value<GuiComboBoxComponent>();
			cb.selected = 2;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->name();
				p.order = index++;
				GuiTextComponent &t = o->value<GuiTextComponent>();
				t.value = Stringizer() + "option " + i;
			}
		}
	}

};

MAIN(guiTestImpl, "combo boxes")
