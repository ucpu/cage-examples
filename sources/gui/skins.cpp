#include "gui.h"

class guiTestImpl : public guiTestClass
{
public:

	void update() override
	{
		entityManagerClass *ents = gui()->entities();

		uint32 animateOption = -1;
		{
			GUI_GET_COMPONENT(comboBox, combo, ents->get(100));
			animateOption = combo.selected;
		}

		static real offset = randomChance() * 1000;
		real t = getApplicationTime() / 5e6f + offset;

		vec4 a4;
		for (uint32 i = 0; i < 4; i++)
			a4[i] = (steeper(rads(t) + rads::Full() * real(i) / 4) * 0.5 + 0.5) * 20;

		skinConfigStruct skin;
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
		guiCreateConfig g;
		g.skinsCount = 2;
		engineCreateConfig e;
		e.gui = &g;
		engineInitialize(e);
	}

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();

		{ // animate option
			entityClass *e = ents->create(100);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 2;
			GUI_GET_COMPONENT(comboBox, input, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "animation";
			static const char *options[] = {
				"margins",
				"borders",
				"paddings",
				"none",
			};
			for (uint32 i = 0; i < sizeof(options)/sizeof(options[0]); i++)
			{
				entityClass *ee = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, ee);
				p.parent = 100;
				p.order = i;
				GUI_GET_COMPONENT(text, text, ee);
				text.value = options[i];
			}
		}
		{ // main
			entityClass *panel = ents->get(3);
			//panel->remove(gui()->components().layoutTable);
			GUI_GET_COMPONENT(layoutLine, ll, panel);
			GUI_GET_COMPONENT(scrollbars, sc, panel);
			sc.alignment = vec2(0.5, 0);
		}
		{ // left panel
			entityClass *panel = ents->create(4);
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = 3;
			p.order = 1;
			GUI_GET_COMPONENT(panel, gp, panel);
			GUI_GET_COMPONENT(layoutLine, ll, panel);
			ll.vertical = true;
		}
		{ // right panel
			entityClass *panel = ents->create(5);
			GUI_GET_COMPONENT(parent, p, panel);
			p.parent = 3;
			p.order = 2;
			GUI_GET_COMPONENT(panel, gp, panel);
			GUI_GET_COMPONENT(layoutLine, ll, panel);
			ll.vertical = true;
			GUI_GET_COMPONENT(widgetState, ws, panel);
			ws.skinIndex = 1;
		}
		for (uint32 side = 4; side < 6; side++)
		{
			uint32 index = 0;
			{ // label
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(label, label, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "People shouldn't be afraid of their government.\nGovernments should be afraid of their people.";
				GUI_GET_COMPONENT(textFormat, format, e);
				format.align = textAlignEnum::Left;
			}
			{ // button
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(button, button, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "button";
			}
			{ // text input box
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(input, input, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "text input box";
			}
			{ // real input box
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(input, input, e);
				input.type = inputTypeEnum::Real;
				input.min.f = -5;
				input.max.f = 5;
				input.step.f = 0.1;
				GUI_GET_COMPONENT(text, text, e);
				text.value = "real input box";
			}
			{ // checkbox
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(checkBox, box, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "checkbox";
			}
			{ // radiobox
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(radioBox, box, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "radiobox";
			}
			{ // combo box
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(comboBox, input, e);
				GUI_GET_COMPONENT(text, text, e);
				text.value = "combo box";
				for (uint32 i = 0; i < 5; i++)
				{
					entityClass *ee = ents->createUnique();
					GUI_GET_COMPONENT(parent, p, ee);
					p.parent = e->name();
					p.order = i;
					GUI_GET_COMPONENT(text, text, ee);
					text.value = i;
				}
			}
			{ // slider
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(sliderBar, slider, e);
			}
			{ // color picker
				entityClass *e = ents->createUnique();
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = side;
				p.order = index++;
				GUI_GET_COMPONENT(colorPicker, picker, e);
			}
		}
	}

};

MAIN(guiTestImpl, "skins")
