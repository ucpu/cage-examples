#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		entityManager *ents = gui()->entities();
		CAGE_COMPONENT_GUI(layoutSplitter, s, ents->get(42));
		switch (name)
		{
		case 3:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			s.vertical = b.state == checkBoxStateEnum::Checked;
		} break;
		case 4:
		{
			CAGE_COMPONENT_GUI(checkBox, b, ents->get(name));
			s.inverse = b.state == checkBoxStateEnum::Checked;
		} break;
		}
	}

	void genContent(sint32 order)
	{
		entityManager *ents = gui()->entities();
		entity *e = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 42;
			p.order = order;
			CAGE_COMPONENT_GUI(text, t, e);
			if (order == 1)
			{
				CAGE_COMPONENT_GUI(panel, panel, e);
				t.value = "Panel";
			}
			else
			{
				CAGE_COMPONENT_GUI(spoiler, spoiler, e);
				t.value = "Spoiler";
			}
			CAGE_COMPONENT_GUI(layoutLine, l, e);
		}
		entity *b = ents->create(100 + order);
		{
			CAGE_COMPONENT_GUI(parent, p, b);
			p.parent = e->name();
			CAGE_COMPONENT_GUI(button, but, b);
			CAGE_COMPONENT_GUI(text, t, b);
			t.value = "Button";
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
			}

			{ // vertical
				entity *e = ents->create(3);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "vertical";
			}

			{ // inverse
				entity *e = ents->create(4);
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				CAGE_COMPONENT_GUI(checkBox, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = "inverse";
			}
		}

		// the test splitter
		entity *presentation = ents->create(42);
		{
			CAGE_COMPONENT_GUI(parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			//CAGE_COMPONENT_GUI(panel, b, presentation);
			CAGE_COMPONENT_GUI(layoutSplitter, l, presentation);
		}

		genContent(1);
		genContent(2);

		//gui()->setZoom(10);
	}

};

MAIN(guiTestImpl, "splitter")
