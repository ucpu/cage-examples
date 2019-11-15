#include "../gui.h"

#include <vector>

class guiTestImpl : public guiTestClass
{
public:

	std::vector<entity *> items;

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		entityManager *ents = gui()->entities();
		CAGE_COMPONENT_GUI(layoutTable, s, ents->get(42));
		switch (name)
		{
		case 2:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			if (b.state == checkBoxStateEnum::Checked)
			{
				CAGE_COMPONENT_GUI(scrollbars, sc, ents->get(42));
			}
			else
				ents->get(42)->remove(gui()->components().scrollbars);
		} break;
		case 3:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			s.vertical = b.state == checkBoxStateEnum::Checked;
		} break;
		case 4:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			s.grid = b.state == checkBoxStateEnum::Checked;
		} break;
		case 5:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			if (b.state == checkBoxStateEnum::Checked)
			{ // add scrollbars
				for (auto e : items)
				{
					CAGE_COMPONENT_GUI(scrollbars, sc, e);
					sc.alignment = randomChance2();
				}
			}
			else
			{ // remove scrollbars
				for (auto e : items)
				{
					e->remove(gui()->components().scrollbars);
				}
			}
		} break;
		case 6:
		{
			CAGE_COMPONENT_GUI(input, b, ents->get(name));
			if (b.valid)
				s.sections = b.value.toUint32();
		} break;
		}
	}

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		entity *mainSplitter = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(layoutSplitter, l, mainSplitter);
			l.vertical = true;
		}

		{
			entity *menu = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(parent, p, menu);
				p.parent = mainSplitter->name();
				p.order = 1;
				CAGE_COMPONENT_GUI(panel, b, menu);
				CAGE_COMPONENT_GUI(layoutLine, l, menu);
				CAGE_COMPONENT_GUI(scrollbars, sc, menu);
			}

			{ // scrollbar
				entity *e = ents->create(2);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 2;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "scrollbar";
			}

			{ // vertical
				entity *e = ents->create(3);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				b.state = checkBoxStateEnum::Checked;
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "vertical";
			}

			{ // grid
				entity *e = ents->create(4);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "grid";
			}

			{ // scrollbars
				entity *e = ents->create(5);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 5;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "scrollbars";
			}

			{ // sections
				entity *e = ents->create(6);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 6;
				CAGE_COMPONENT_GUI(input, b, e);
				b.min.i = 0;
				b.max.i = 10;
				b.step.i = 1;
				b.type = inputTypeEnum::Integer;
				b.value = "0";
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "sections";
			}
		}

		// the test table
		entity *presentation = ents->create(42);
		{
			CAGE_COMPONENT_GUI(parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			CAGE_COMPONENT_GUI(panel, b, presentation);
			CAGE_COMPONENT_GUI(layoutTable, t, presentation);
			t.sections = 0;
		}

		// table content
		for (uint32 i = 0; i < 17; i++)
		{
			// scrollbars
			entity *o = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(parent, p, o);
				p.parent = 42;
				p.order = i;
				CAGE_COMPONENT_GUI(panel, panel, o);
			}
			items.push_back(o);
			// button
			entity *b = ents->createUnique();
			{
				CAGE_COMPONENT_GUI(parent, p, b);
				p.parent = o->name();
				CAGE_COMPONENT_GUI(button, but, b);
				CAGE_COMPONENT_GUI(text, t, b);
				t.value = stringizer() + "item " + i;
				CAGE_COMPONENT_GUI(explicitSize, size, b);
				size.size = randomRange2(50, 250);
			}
		}
	}

};

MAIN(guiTestImpl, "table")
