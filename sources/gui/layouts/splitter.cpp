#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		entityManagerClass *ents = gui()->entities();
		GUI_GET_COMPONENT(layoutSplitter, s, ents->get(42));
		switch (name)
		{
		case 3:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			s.vertical = b.state == checkBoxStateEnum::Checked;
		} break;
		case 4:
		{
			GUI_GET_COMPONENT(checkBox, b, ents->get(name));
			s.inverse = b.state == checkBoxStateEnum::Checked;
		} break;
		}
	}

	void genContent(sint32 order)
	{
		entityManagerClass *ents = gui()->entities();
		entityClass *e = ents->createUnique();
		{
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 42;
			p.order = order;
			GUI_GET_COMPONENT(text, t, e);
			if (order == 1)
			{
				GUI_GET_COMPONENT(panel, panel, e);
				t.value = "Panel";
			}
			else
			{
				GUI_GET_COMPONENT(spoiler, spoiler, e);
				t.value = "Spoiler";
			}
			GUI_GET_COMPONENT(layoutLine, l, e);
		}
		entityClass *b = ents->create(100 + order);
		{
			GUI_GET_COMPONENT(parent, p, b);
			p.parent = e->name();
			GUI_GET_COMPONENT(button, but, b);
			GUI_GET_COMPONENT(text, t, b);
			t.value = "Button";
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
			}

			{ // vertical
				entityClass *e = ents->create(3);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				GUI_GET_COMPONENT(checkBox, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = "vertical";
			}

			{ // inverse
				entityClass *e = ents->create(4);
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				GUI_GET_COMPONENT(checkBox, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = "inverse";
			}
		}

		// the test splitter
		entityClass *presentation = ents->create(42);
		{
			GUI_GET_COMPONENT(parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			//GUI_GET_COMPONENT(panel, b, presentation);
			GUI_GET_COMPONENT(layoutSplitter, l, presentation);
		}

		genContent(1);
		genContent(2);

		//gui()->setZoom(10);
	}

};

MAIN(guiTestImpl, "splitter")
