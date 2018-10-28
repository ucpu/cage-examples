#include "../gui.h"

#include <vector>

class guiTestImpl : public guiTestClass
{
public:

	std::vector<entityClass *> items;

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		entityManagerClass *ents = gui()->entities();
		GUI_GET_COMPONENT(layoutTable, s, ents->get(42));
		switch (name)
		{
		case 2:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			if (b.state == checkBoxStateEnum::Checked)
			{
				GUI_GET_COMPONENT(scrollbars, sc, ents->get(42));
			}
			else
				ents->get(42)->remove(gui()->components().scrollbars);
		} break;
		case 3:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			s.vertical = b.state == checkBoxStateEnum::Checked;
		} break;
		case 4:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			s.grid = b.state == checkBoxStateEnum::Checked;
		} break;
		case 5:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			if (b.state == checkBoxStateEnum::Checked)
			{ // add scrollbars
				for (auto e : items)
				{
					GUI_GET_COMPONENT(scrollbars, sc, e);
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
			GUI_GET_COMPONENT(input, b, ents->get(name));
			if (b.valid)
				s.sections = b.value.toUint32();
		} break;
		}
	}

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		entityClass *mainSplitter = ents->createUnique();
		{
			GUI_GET_COMPONENT(layoutSplitter, l, mainSplitter);
			l.vertical = true;
		}

		{
			entityClass *menu = ents->createUnique();
			{
				GUI_GET_COMPONENT(parent, p, menu);
				p.parent = mainSplitter->name();
				p.order = 1;
				GUI_GET_COMPONENT(panel, b, menu);
				GUI_GET_COMPONENT(layoutLine, l, menu);
				GUI_GET_COMPONENT(scrollbars, sc, menu);
			}

			{ // scrollbar
				entityClass *e = ents->create(2);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 2;
				GUI_GET_COMPONENT(checkBox, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = "scrollbar";
			}

			{ // vertical
				entityClass *e = ents->create(3);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				GUI_GET_COMPONENT(checkBox, b, e);
				b.state = checkBoxStateEnum::Checked;
				GUI_GET_COMPONENT(text, t, e);
				t.value = "vertical";
			}

			{ // grid
				entityClass *e = ents->create(4);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				GUI_GET_COMPONENT(checkBox, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = "grid";
			}

			{ // scrollbars
				entityClass *e = ents->create(5);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 5;
				GUI_GET_COMPONENT(checkBox, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = "scrollbars";
			}

			{ // sections
				entityClass *e = ents->create(6);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 6;
				GUI_GET_COMPONENT(input, b, e);
				b.min.i = 0;
				b.max.i = 10;
				b.step.i = 1;
				b.type = inputTypeEnum::Integer;
				b.value = 0;
				GUI_GET_COMPONENT(text, t, e);
				t.value = "sections";
			}
		}

		// the test table
		entityClass *presentation = ents->create(42);
		{
			GUI_GET_COMPONENT(parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			GUI_GET_COMPONENT(panel, b, presentation);
			GUI_GET_COMPONENT(layoutTable, t, presentation);
			t.sections = 0;
		}

		// table content
		for (uint32 i = 0; i < 17; i++)
		{
			// scrollbars
			entityClass *o = ents->createUnique();
			{
				GUI_GET_COMPONENT(parent, p, o);
				p.parent = 42;
				p.order = i;
				GUI_GET_COMPONENT(panel, panel, o);
			}
			items.push_back(o);
			// button
			entityClass *b = ents->createUnique();
			{
				GUI_GET_COMPONENT(parent, p, b);
				p.parent = o->name();
				GUI_GET_COMPONENT(button, but, b);
				GUI_GET_COMPONENT(text, t, b);
				t.value = string("item ") + i;
				GUI_GET_COMPONENT(explicitSize, size, b);
				size.size = randomRange2(50, 250);
			}
		}
	}

};

MAIN(guiTestImpl, "table")
