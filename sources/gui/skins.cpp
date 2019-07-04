#include "gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void update() override
	{
		entityManager *ents = gui()->entities();

		uint32 animateOption = -1;
		{
			CAGE_COMPONENT_GUI(comboBox, combo, ents->get(100));
			animateOption = combo.selected;
		}

		static real offset = randomChance() * 1000;
		real t = getApplicationTime() / 5e6f + offset;

		vec4 a4;
		for (uint32 i = 0; i < 4; i++)
			a4[i] = (steeper(rads(t) + rads::Full() * real(i) / 4) * 0.5 + 0.5) * 20;

		guiSkinConfig skin;
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
			for (uint32 i = 0; i < (uint32)elementTypeEnum::TotalElements; i++)
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
		gui()->skin(0) = gui()->skin(1) = skin;
		gui()->skin(1).textureName = hashString("cage-tests/gui/skin.png");
	}

	void initializeEngine() override
	{
		guiManagerCreateConfig g;
		g.skinsCount = 2;
		engineCreateConfig e;
		e.gui = &g;
		engineInitialize(e);
	}

	void initialize() override
	{
		guiBasicLayout();

		entityManager *ents = gui()->entities();

		{ // animate option
			entity *e = ents->create(100);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 2;
			CAGE_COMPONENT_GUI(comboBox, input, e);
			CAGE_COMPONENT_GUI(text, text, e);
			text.value = "animation";
			static const char *options[] = {
				"margins",
				"borders",
				"paddings",
				"none",
			};
			for (uint32 i = 0; i < sizeof(options)/sizeof(options[0]); i++)
			{
				entity *ee = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, ee);
				p.parent = 100;
				p.order = i;
				CAGE_COMPONENT_GUI(text, text, ee);
				text.value = options[i];
			}
		}
		{ // main
			entity *panel = ents->get(3);
			//panel->remove(gui()->components().layoutTable);
			CAGE_COMPONENT_GUI(layoutLine, ll, panel);
			CAGE_COMPONENT_GUI(scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}
		{ // left panel
			entity *panel = ents->create(4);
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = 3;
			p.order = 1;
			CAGE_COMPONENT_GUI(panel, gp, panel);
			CAGE_COMPONENT_GUI(layoutLine, ll, panel);
			ll.vertical = true;
		}
		{ // right panel
			entity *panel = ents->create(5);
			CAGE_COMPONENT_GUI(parent, p, panel);
			p.parent = 3;
			p.order = 2;
			CAGE_COMPONENT_GUI(panel, gp, panel);
			CAGE_COMPONENT_GUI(layoutLine, ll, panel);
			ll.vertical = true;
			CAGE_COMPONENT_GUI(widgetState, ws, panel);
			ws.skinIndex = 1;
		}
		for (uint32 side = 4; side < 6; side++)
		{
			uint32 index = 0;
			{ // label
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(label, label, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "People shouldn't be afraid of their government.\nGovernments should be afraid of their people.";
				CAGE_COMPONENT_GUI(textFormat, format, e);
				format.align = textAlignEnum::Left;
			}
			{ // button
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(button, button, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "button";
			}
			{ // text input box
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(input, input, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "text input box";
			}
			{ // real input box
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(input, input, e);
				input.type = inputTypeEnum::Real;
				input.min.f = -5;
				input.max.f = 5;
				input.step.f = 0.1;
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "real input box";
			}
			{ // checkbox
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(checkBox, box, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "checkbox";
			}
			{ // radiobox
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(radioBox, box, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "radiobox";
			}
			{ // combo box
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(comboBox, input, e);
				CAGE_COMPONENT_GUI(text, text, e);
				text.value = "combo box";
				for (uint32 i = 0; i < 5; i++)
				{
					entity *ee = ents->createUnique();
					CAGE_COMPONENT_GUI(parent, p, ee);
					p.parent = e->name();
					p.order = i;
					CAGE_COMPONENT_GUI(text, text, ee);
					text.value = i;
				}
			}
			{ // slider
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(sliderBar, slider, e);
			}
			{ // color picker
				entity *e = ents->createUnique();
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = side;
				p.order = index++;
				CAGE_COMPONENT_GUI(colorPicker, picker, e);
			}
		}
	}

};

MAIN(guiTestImpl, "skins")
