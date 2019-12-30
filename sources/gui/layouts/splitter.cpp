#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		EntityManager *ents = gui()->entities();
		CAGE_COMPONENT_GUI(LayoutSplitter, s, ents->get(42));
		switch (name)
		{
		case 3:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			s.vertical = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 4:
		{
			CAGE_COMPONENT_GUI(CheckBox, b, ents->get(name));
			s.inverse = b.state == CheckBoxStateEnum::Checked;
		} break;
		}
	}

	void genContent(sint32 order)
	{
		EntityManager *ents = gui()->entities();
		Entity *e = ents->createUnique();
		{
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 42;
			p.order = order;
			CAGE_COMPONENT_GUI(Text, t, e);
			if (order == 1)
			{
				CAGE_COMPONENT_GUI(Panel, panel, e);
				t.value = "Panel";
			}
			else
			{
				CAGE_COMPONENT_GUI(Spoiler, spoiler, e);
				t.value = "Spoiler";
			}
			CAGE_COMPONENT_GUI(LayoutLine, l, e);
		}
		Entity *b = ents->create(100 + order);
		{
			CAGE_COMPONENT_GUI(Parent, p, b);
			p.parent = e->name();
			CAGE_COMPONENT_GUI(Button, but, b);
			CAGE_COMPONENT_GUI(Text, t, b);
			t.value = "Button";
		}
	}

	void initialize() override
	{
		EntityManager *ents = gui()->entities();

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
			}

			{ // vertical
				Entity *e = ents->create(3);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 3;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "vertical";
			}

			{ // inverse
				Entity *e = ents->create(4);
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = menu->name();
				p.order = 4;
				CAGE_COMPONENT_GUI(CheckBox, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = "inverse";
			}
		}

		// the test splitter
		Entity *presentation = ents->create(42);
		{
			CAGE_COMPONENT_GUI(Parent, p, presentation);
			p.parent = mainSplitter->name();
			p.order = 2;
			//CAGE_COMPONENT_GUI(Panel, b, presentation);
			CAGE_COMPONENT_GUI(LayoutSplitter, l, presentation);
		}

		genContent(1);
		genContent(2);

		//gui()->setZoom(10);
	}

};

MAIN(guiTestImpl, "splitter")
