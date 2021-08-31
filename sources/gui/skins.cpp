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
			GuiComboBoxComponent &combo = ents->get(100)->value<GuiComboBoxComponent>();
			animateOption = combo.selected;
		}

		static Real offset = randomChance() * 1000;
		Real t = applicationTime() / 5e6f + offset;

		Vec4 a4;
		for (uint32 i = 0; i < 4; i++)
			a4[i] = (steeper(Rads(t) + Rads::Full() * Real(i) / 4) * 0.5 + 0.5) * 20;

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
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 2;
			GuiComboBoxComponent &input = e->value<GuiComboBoxComponent>();
			GuiTextComponent &text = e->value<GuiTextComponent>();
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
				GuiParentComponent &p = ee->value<GuiParentComponent>();
				p.parent = 100;
				p.order = i;
				GuiTextComponent &text = ee->value<GuiTextComponent>();
				text.value = options[i];
			}
		}
		{ // main
			Entity *panel = ents->get(3);
			//panel->remove(gui()->components().LayoutTable);
			GuiLayoutLineComponent &ll = panel->value<GuiLayoutLineComponent>();
			GuiScrollbarsComponent &sc = panel->value<GuiScrollbarsComponent>();
			sc.alignment = Vec2(0.5, 0);
		}
		{ // left panel
			Entity *panel = ents->create(4);
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = 3;
			p.order = 1;
			GuiPanelComponent &gp = panel->value<GuiPanelComponent>();
			GuiLayoutLineComponent &ll = panel->value<GuiLayoutLineComponent>();
			ll.vertical = true;
		}
		{ // right panel
			Entity *panel = ents->create(5);
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = 3;
			p.order = 2;
			GuiPanelComponent &gp = panel->value<GuiPanelComponent>();
			GuiLayoutLineComponent &ll = panel->value<GuiLayoutLineComponent>();
			ll.vertical = true;
			GuiWidgetStateComponent &ws = panel->value<GuiWidgetStateComponent>();
			ws.skinIndex = 1;
		}
		for (uint32 side = 4; side < 6; side++)
		{
			uint32 index = 0;
			{ // label
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiLabelComponent &label = e->value<GuiLabelComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "People shouldn't be afraid of their government.\nGovernments should be afraid of their people.";
				GuiTextFormatComponent &format = e->value<GuiTextFormatComponent>();
				format.align = TextAlignEnum::Left;
			}
			{ // button
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiButtonComponent &button = e->value<GuiButtonComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "button";
			}
			{ // text input box
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiInputComponent &input = e->value<GuiInputComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "text input box";
			}
			{ // real input box
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiInputComponent &input = e->value<GuiInputComponent>();
				input.type = InputTypeEnum::Real;
				input.min.f = -5;
				input.max.f = 5;
				input.step.f = 0.1;
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "real input box";
			}
			{ // checkbox
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiCheckBoxComponent &box = e->value<GuiCheckBoxComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "checkbox";
			}
			{ // radiobox
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiRadioBoxComponent &box = e->value<GuiRadioBoxComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "radiobox";
			}
			{ // combo box
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiComboBoxComponent &input = e->value<GuiComboBoxComponent>();
				GuiTextComponent &text = e->value<GuiTextComponent>();
				text.value = "combo box";
				for (uint32 i = 0; i < 5; i++)
				{
					Entity *ee = ents->createUnique();
					GuiParentComponent &p = ee->value<GuiParentComponent>();
					p.parent = e->name();
					p.order = i;
					GuiTextComponent &text = ee->value<GuiTextComponent>();
					text.value = Stringizer() + i;
				}
			}
			{ // slider
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiSliderBarComponent &slider = e->value<GuiSliderBarComponent>();
			}
			{ // color picker
				Entity *e = ents->createUnique();
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = side;
				p.order = index++;
				GuiColorPickerComponent &picker = e->value<GuiColorPickerComponent>();
			}
		}
	}

};

MAIN(guiTestImpl, "skins")
