#include "../gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void guiEvent(uint32 name) override
	{
		guiTestClass::guiEvent(name);
		EntityManager *ents = engineGuiEntities();
		GuiLayoutSplitterComponent &s = ents->get(42)->value<GuiLayoutSplitterComponent>();
		switch (name)
		{
		case 3:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
			s.vertical = b.state == CheckBoxStateEnum::Checked;
		} break;
		case 4:
		{
			GuiCheckBoxComponent &b = ents->get(name)->value<GuiCheckBoxComponent>();
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
				GuiPanelComponent &panel = e->value<GuiPanelComponent>();
				t.value = "Panel";
			}
			else
			{
				GuiSpoilerComponent &spoiler = e->value<GuiSpoilerComponent>();
				t.value = "Spoiler";
			}
			GuiLayoutLineComponent &l = e->value<GuiLayoutLineComponent>();
		}
		Entity *b = ents->create(100 + order);
		{
			GuiParentComponent &p = b->value<GuiParentComponent>();
			p.parent = e->name();
			GuiButtonComponent &but = b->value<GuiButtonComponent>();
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
				GuiPanelComponent &b = menu->value<GuiPanelComponent>();
				GuiLayoutLineComponent &l = menu->value<GuiLayoutLineComponent>();
			}

			{ // vertical
				Entity *e = ents->create(3);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 3;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = "vertical";
			}

			{ // inverse
				Entity *e = ents->create(4);
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = menu->name();
				p.order = 4;
				GuiCheckBoxComponent &b = e->value<GuiCheckBoxComponent>();
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
			
			GuiLayoutSplitterComponent &l = presentation->value<GuiLayoutSplitterComponent>();
		}

		genContent(1);
		genContent(2);

		//gui()->setZoom(10);
	}

};

MAIN(guiTestImpl, "splitter")
