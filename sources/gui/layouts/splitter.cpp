#include "../gui.h"

class GuiTestImpl : public GuiTestClass
{
public:
	void guiEvent(InputGuiWidget in) override
	{
		GuiTestClass::guiEvent(in);
		EntityManager *ents = engineGuiEntities();
		GuiLayoutSplitterComponent &s = ents->get(42)->value<GuiLayoutSplitterComponent>();
		switch (in.widget)
		{
		case 3:
		{
			GuiCheckBoxComponent &b = ents->get(in.widget)->value<GuiCheckBoxComponent>();
			s.vertical = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 4:
		{
			GuiCheckBoxComponent &b = ents->get(in.widget)->value<GuiCheckBoxComponent>();
			s.inverse = b.state == CheckBoxStateEnum::Checked;
		} break;
		}
	}

	void genContent(sint32 order)
	{
		EntityManager *ents = engineGuiEntities();
		Entity *e = ents->createUnique();
		{
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 42;
			p.order = order;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			if (order == 1)
			{
				e->value<GuiPanelComponent>();
				t.value = "Panel";
			}
			else
			{
				e->value<GuiSpoilerComponent>();
				t.value = "Spoiler";
			}
			e->value<GuiLayoutLineComponent>();
		}
		Entity *b = ents->create(100 + order);
		{
			GuiParentComponent &p = b->value<GuiParentComponent>();
			p.parent = e->name();
			b->value<GuiButtonComponent>();
			GuiTextComponent &t = b->value<GuiTextComponent>();
			t.value = "Button";
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

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
				menu->value<GuiPanelComponent>();
				menu->value<GuiLayoutLineComponent>();
			}

			{ // vertical
				Entity *e = ents->create(3);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 3;
				e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "vertical";
			}

			{ // inverse
				Entity *e = ents->create(4);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 4;
				e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "inverse";
			}
		}

		// the test splitter
		Entity *presentation = ents->create(42);
		{
			GuiParentComponent &p = presentation->value<GuiParentComponent>();
			p.parent = mainSplitter->name();
			p.order = 2;

			presentation->value<GuiLayoutSplitterComponent>();
		}

		genContent(1);
		genContent(2);

		//gui()->setZoom(10);
	}
};

MAIN(GuiTestImpl, "splitter")
