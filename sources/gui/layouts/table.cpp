#include "../gui.h"

#include <vector>

class guiTestImpl : public guiTestClass
{
public:

	std::vector<Entity *> items;

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		EntityManager *ents = engineGui()->entities();
		CAGE_COMPONENT_GUI(LayoutTable, s, ents->get(42));
		switch (name)
		{
		case 2:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			if (b.state == CheckBoxStateEnum::Checked)
			{
				CAGE_COMPONENT_GUI(Scrollbars, sc, ents->get(42));
			}
			else
				ents->get(42)->remove(engineGui()->components().Scrollbars);
		} break;
		case 3:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			s.vertical = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 4:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			s.grid = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 5:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			if (b.state == CheckBoxStateEnum::Checked)
			{ // add scrollbars
				for (auto e : items)
				{
					CAGE_COMPONENT_GUI(Scrollbars, sc, e);
					sc.alignment = randomChance2();
				}
			}
			else
			{ // remove scrollbars
				for (auto e : items)
				{
					e->remove(engineGui()->components().Scrollbars);
				}
			}
		} break;
		case 6:
		{
			CAGE_COMPONENT_GUI(Input, b, ents->get(name));
			if (b.valid)
				s.sections = b.value.toUint32();
		} break;
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		Entity *mainSplitter = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(LayoutSplitter, l, mainSplitter);
			l.vertical = true;
		}

		{
			Entity *menu = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(Parent, p, menu);
				p.parent = mainSplitter->name();
				p.order = 1;
				CAGE_COMPONENT_GUI(Panel, b, menu);
				CAGE_COMPONENT_GUI(LayoutLine, l, menu);
				CAGE_COMPONENT_GUI(Scrollbars, sc, menu);
			}

			{ // scrollbar
				Entity *e = ents->create(2);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 2;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "scrollbar";
			}

			{ // vertical
				Entity *e = ents->create(3);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				b.state = CheckBoxStateEnum::Checked;
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "vertical";
			}

			{ // grid
				Entity *e = ents->create(4);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "grid";
			}

			{ // scrollbars
				Entity *e = ents->create(5);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 5;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "scrollbars";
			}

			{ // sections
				Entity *e = ents->create(6);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 6;
				CAGE_COMPONENT_GUI(Input, b, e);
				b.min.i = 0;
				b.max.i = 10;
				b.step.i = 1;
				b.type = InputTypeEnum::Integer;
				b.value = "0";
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "sections";
			}
		}

		// the test table
		Entity *presentation = ents->create(42);
		{
			CAGE_COMPONENT_GUI(Parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(Panel, b, presentation);
			CAGE_COMPONENT_GUI(LayoutTable, t, presentation);
			t.sections = 0;
		}

		// table content
		for (uint32 i = 0; i < 17; i++)
		{
			// scrollbars
			Entity *o = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(Parent, p, o);
				p.parent = 42;
				p.order = i;
				CAGE_COMPONENT_GUI(Panel, panel, o);
			}
			items.push_back(o);
			// button
			Entity *b = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(Parent, p, b);
				p.parent = o->name();
				CAGE_COMPONENT_GUI(Button, but, b);
				CAGE_COMPONENT_GUI(Text, t, b);
				t.value = stringizer() + "item " + i;
				CAGE_COMPONENT_GUI(ExplicitSize, size, b);
				size.size = randomRange2(50, 250);
			}
		}
	}

};

MAIN(guiTestImpl, "table")
