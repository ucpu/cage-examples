#include <cage-engine/guiSkins.h>

#include "gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void update() override
	{
		EntityManager *ents = engineGui()->entities();

		uint32 animateOption = -1;
		{
			CAGE_COMPONENT_GUI(ComboBox, combo, ents->get(100));
			animateOption = combo.selected;
		}

		static real offset = randomChance() * 1000;
		real t = getApplicationTime() / 5e6f + offset;

		vec4 a4;
		for (uint32 i = 0; i < 4; i++)
			a4[i] = (steeper(rads(t) + rads::Full() * real(i) / 4) * 0.5 + 0.5) * 20;

		GuiSkinConfig skin;
		switch (animateOption)
		{
		case 0: // margins
			skin.defaults.button.margin = a4;
			skin.defaults.checkBox.margin = a4;
			skin.defaults.radioBox.margin = a4;
			skin.defaults.colorPicker.margin = a4;
			skin.defaults.comboBox.baseMargin = a4;
			skin.defaults.panel.baseMargin = a4;
			skin.defaults.inputBox.margin = a4;
			skin.defaults.label.margin = a4;
			skin.defaults.sliderBar.horizontal.margin = a4;
			break;
		case 1: // borders
			for (uint32 i = 0; i < (uint32)GuiElementTypeEnum::TotalElements; i++)
				skin.layouts[i].border = a4;
			break;
		case 2: // paddings
			skin.defaults.button.padding = a4;
			skin.defaults.comboBox.basePadding = a4;
			skin.defaults.panel.captionPadding = a4;
			skin.defaults.panel.contentPadding = a4;
			skin.defaults.inputBox.basePadding = a4;
			skin.defaults.sliderBar.horizontal.padding = a4;
			break;
		}
		engineGui()->skin(0) = engineGui()->skin(1) = skin;
		engineGui()->skin(1).textureName = HashString("cage-tests/gui/skin.png");
	}

	void initializeEngine() override
	{
		GuiCreateConfig g;
		g.skinsCount = 2;
		EngineCreateConfig e;
		e.gui = &g;
		engineInitialize(e);
	}

	void initialize() override
	{
		guiBasicLayout();

		EntityManager *ents = engineGui()->entities();

		{ // animate option
			Entity *e = ents->create(100);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 2;
			CAGE_COMPONENT_GUI(ComboBox, input, e);
			CAGE_COMPONENT_GUI(Text, text, e);
			text.value = "animation";
			static const char *options[] = {
				"margins",
				"borders",
				"paddings",
				"none",
			};
			for (uint32 i = 0; i < sizeof(options)/sizeof(options[0]); i++)
			{
				Entity *ee = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, ee);
				p.parent = 100;
				p.order = i;
				CAGE_COMPONENT_GUI(Text, text, ee);
				text.value = options[i];
			}
		}
		{ // main
			Entity *panel = ents->get(3);
			//panel->remove(gui()->components().LayoutTable);
			CAGE_COMPONENT_GUI(LayoutLine, ll, panel);
			CAGE_COMPONENT_GUI(Scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}
		{ // left panel
			Entity *panel = ents->create(4);
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = 3;
			p.order = 1;
			CAGE_COMPONENT_GUI(Panel, gp, panel);
			CAGE_COMPONENT_GUI(LayoutLine, ll, panel);
			ll.vertical = true;
		}
		{ // right panel
			Entity *panel = ents->create(5);
			CAGE_COMPONENT_GUI(Parent, p, panel);
			p.parent = 3;
			p.order = 2;
			CAGE_COMPONENT_GUI(Panel, gp, panel);
			CAGE_COMPONENT_GUI(LayoutLine, ll, panel);
			ll.vertical = true;
			CAGE_COMPONENT_GUI(WidgetState, ws, panel);
			ws.skinIndex = 1;
		}
		for (uint32 side = 4; side < 6; side++)
		{
			uint32 index = 0;
			{ // label
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(Label, label, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "People shouldn't be afraid of their government.\nGovernments should be afraid of their people.";
				CAGE_COMPONENT_GUI(TextFormat, format, e);
				format.align = TextAlignEnum::Left;
			}
			{ // button
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(Button, button, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "button";
			}
			{ // text input box
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(Input, input, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "text input box";
			}
			{ // real input box
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(Input, input, e);
				input.type = InputTypeEnum::Real;
				input.min.f = -5;
				input.max.f = 5;
				input.step.f = 0.1;
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "real input box";
			}
			{ // checkbox
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(CheckBox, box, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "checkbox";
			}
			{ // radiobox
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(RadioBox, box, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "radiobox";
			}
			{ // combo box
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(ComboBox, input, e);
				CAGE_COMPONENT_GUI(Text, text, e);
				text.value = "combo box";
				for (uint32 i = 0; i < 5; i++)
				{
					Entity *ee = ents->createUnique();
					CAGE_COMPONENT_GUI(Parent, p, ee);
					p.parent = e->name();
					p.order = i;
					CAGE_COMPONENT_GUI(Text, text, ee);
					text.value = stringizer() + i;
				}
			}
			{ // slider
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(SliderBar, slider, e);
			}
			{ // color picker
				Entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(ColorPicker, picker, e);
			}
		}
	}

};

MAIN(guiTestImpl, "skins")
