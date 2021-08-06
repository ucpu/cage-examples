#include "../gui.h"
#include <cage-core/string.h>

#include <vector>

class guiTestImpl : public guiTestClass
{
public:

	std::vector<Entity *> items;

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		EntityManager *ents = engineGui()->entities();
		GuiLayoutTableComponent &s = ents->get(42)->value<GuiLayoutTableComponent>();
		switch (name)
		{
		case 2:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
			if (b.state == CheckBoxStateEnum::Checked)
			{
				GuiScrollbarsComponent &sc = ents->get(42)->value<GuiScrollbarsComponent>();
			}
			else
				ents->get(42)->remove<GuiScrollbarsComponent>();
		} break;
		case 3:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
			s.vertical = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 4:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
			s.grid = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 5:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
			if (b.state == CheckBoxStateEnum::Checked)
			{ // add scrollbars
				for (auto e : items)
				{
					GuiScrollbarsComponent &sc = e->value<GuiScrollbarsComponent>();
					sc.alignment = randomChance2();
				}
			}
			else
			{ // remove scrollbars
				for (auto e : items)
				{
					e->remove<GuiScrollbarsComponent>();
				}
			}
		} break;
		case 6:
		{
			GuiInputComponent &b = ents->get(name)->value<GuiInputComponent>();
			if (b.valid)
				s.sections = toUint32(b.value);
		} break;
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		Entity *mainSplitter = ents->createUnique();
		{
			GuiLayoutSplitterComponent &l = mainSplitter->value<GuiLayoutSplitterComponent>();
			l.vertical = true;
		}

		{
			Entity *menu = ents->createUnique();
			{
				GuiParentComponent &p = menu->value<GuiParentComponent>();
				p.parent = mainSplitter->name();
				p.order = 1;
				GuiPanelComponent &b = menu->value<GuiPanelComponent>();
				GuiLayoutLineComponent &l = menu->value<GuiLayoutLineComponent>();
				GuiScrollbarsComponent &sc = menu->value<GuiScrollbarsComponent>();
			}

			{ // scrollbar
				Entity *e = ents->create(2);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 2;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "scrollbar";
			}

			{ // vertical
				Entity *e = ents->create(3);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 3;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
				b.state = CheckBoxStateEnum::Checked;
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "vertical";
			}

			{ // grid
				Entity *e = ents->create(4);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 4;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "grid";
			}

			{ // scrollbars
				Entity *e = ents->create(5);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 5;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "scrollbars";
			}

			{ // sections
				Entity *e = ents->create(6);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 6;
				GuiInputComponent &b = e->value<GuiInputComponent>();
				b.min.i = 0;
				b.max.i = 10;
				b.step.i = 1;
				b.type = InputTypeEnum::Integer;
				b.value = "0";
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "sections";
			}
		}

		// the test table
		Entity *presentation = ents->create(42);
		{
			GuiParentComponent &p = presentation->value<GuiParentComponent>();
			p.parent = mainSplitter->name();
			p.order = 2;
			GuiPanelComponent &b = presentation->value<GuiPanelComponent>();
			GuiLayoutTableComponent &t = presentation->value<GuiLayoutTableComponent>();
			t.sections = 0;
		}

		// table content
		for (uint32 i = 0; i < 17; i++)
		{
			// scrollbars
			Entity *o = ents->createUnique();
			{
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = 42;
				p.order = i;
				GuiPanelComponent &panel = o->value<GuiPanelComponent>();
			}
			items.push_back(o);
			// button
			Entity *b = ents->createUnique();
			{
				GuiParentComponent &p = b->value<GuiParentComponent>();
				p.parent = o->name();
				GuiButtonComponent &but = b->value<GuiButtonComponent>();
				GuiTextComponent &t = b->value<GuiTextComponent>();
				t.value = stringizer() + "item " + i;
				GuiExplicitSizeComponent &size = b->value<GuiExplicitSizeComponent>();
				size.size = randomRange2(50, 250);
			}
		}
	}

};

MAIN(guiTestImpl, "table")
