#include "gui.h"

#include <cage-core/memoryBuffer.h>
#include <cage-engine/guiSkins.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	static MemoryBuffer makeBufferWithText()
	{
		static constexpr const char str[] = "Alea iacta est\nThe die is cast\nVeni vidi vici\nI came, I saw, I conquered\nAve Imperator, morituri te salutant\nHail, Emperor, those who are about to die salute you";
		MemoryBuffer buf;
		buf.resize(sizeof(str));
		detail::memcpy(buf.data(), str, sizeof(str));
		return buf;
	}

	MemoryBuffer buffer = makeBufferWithText();

	void update() override
	{
		EntityManager *ents = engineGuiEntities();

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

		const auto &generate = [&](GuiSkinIndex style)
		{
			GuiSkinConfig skin = detail::guiSkinGenerate(style);
			switch (animateOption)
			{
				case 0: // margins
					skin.defaults.label.margin = a4;
					skin.defaults.button.margin = a4;
					skin.defaults.inputBox.margin = a4;
					skin.defaults.textArea.margin = a4;
					skin.defaults.checkBox.margin = a4;
					skin.defaults.radioBox.margin = a4;
					skin.defaults.comboBox.baseMargin = a4;
					skin.defaults.progressBar.baseMargin = a4;
					skin.defaults.sliderBar.horizontal.margin = a4;
					skin.defaults.colorPicker.margin = a4;
					skin.defaults.panel.baseMargin = a4;
					skin.defaults.spoiler.baseMargin = a4;
					break;
				case 1: // borders
					for (uint32 i = 0; i < (uint32)GuiElementTypeEnum::TotalElements; i++)
						skin.layouts[i].border = a4;
					break;
				case 2: // paddings
					skin.defaults.button.padding = a4;
					skin.defaults.inputBox.basePadding = a4;
					skin.defaults.textArea.padding = a4;
					skin.defaults.comboBox.basePadding = a4;
					skin.defaults.progressBar.textPadding = a4;
					skin.defaults.progressBar.fillingPadding = a4;
					skin.defaults.sliderBar.horizontal.padding = a4;
					skin.defaults.panel.captionPadding = a4;
					skin.defaults.panel.contentPadding = a4;
					skin.defaults.spoiler.captionPadding = a4;
					skin.defaults.spoiler.contentPadding = a4;
					break;
			}
			return skin;
		};

		engineGuiManager()->skin(GuiSkinDefault) = generate(GuiSkinDefault);
		engineGuiManager()->skin(GuiSkinTooltips) = generate(GuiSkinTooltips);
		engineGuiManager()->skin(GuiSkinIndex(2)) = generate(GuiSkinDefault);
		engineGuiManager()->skin(GuiSkinIndex(2)).textureId = HashString("cage-tests/gui/skin.png");
	}

	void initializeEngine() override
	{
		GuiManagerCreateConfig g;
		g.skinsCount = 3;
		EngineCreateConfig e;
		e.gui = &g;
		engineInitialize(e);
	}

	void initializeSide(uint32 side, const String &name)
	{
		EntityManager *ents = engineGuiEntities();
		{ // panel
			Entity *panel = ents->create(side);
			GuiParentComponent &p = panel->value<GuiParentComponent>();
			p.parent = 3;
			p.order = side;
			panel->value<GuiPanelComponent>();
			panel->value<GuiLayoutLineComponent>().vertical = true;
			panel->value<GuiLayoutLineComponent>().last = LineEdgeModeEnum::Empty;
			panel->value<GuiTextComponent>().value = name;
			panel->value<GuiLayoutScrollbarsComponent>();
		}
		uint32 index = 0;
		{ // label 1
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "People shouldn't be afraid of their government.\nGovernments should be afraid of their people.";
			e->value<GuiTextFormatComponent>().align = TextAlignEnum::Left;
		}
		{ // label 2
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "Remember, remember the fifth of November\nof gunpowder treason and plot.\nI know of no reason why the gun powder\ntreason should ever be forgot.";
			e->value<GuiTextFormatComponent>().align = TextAlignEnum::Left;
		}
		{ // button
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "button";
		}
		{ // text input box
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiInputComponent>();
			e->value<GuiTextComponent>().value = "text input box";
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
			e->value<GuiTextComponent>().value = "real input box";
		}
		{ // disabled input box
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			GuiInputComponent &input = e->value<GuiInputComponent>();
			input.type = InputTypeEnum::Real;
			input.min.f = -5;
			input.max.f = 5;
			input.step.f = 0.1;
			e->value<GuiTextComponent>().value = "disabled input box";
			e->value<GuiWidgetStateComponent>().disabled = true;
		}
		{ // checkbox
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiCheckBoxComponent>();
			e->value<GuiTextComponent>().value = "checkbox";
		}
		{ // radiobox
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiRadioBoxComponent>();
			e->value<GuiTextComponent>().value = "radiobox";
		}
		{ // combo box
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "combo box";
			for (uint32 i = 0; i < 5; i++)
			{
				Entity *ee = ents->createUnique();
				GuiParentComponent &p = ee->value<GuiParentComponent>();
				p.parent = e->id();
				p.order = i;
				ee->value<GuiTextComponent>().value = Stringizer() + i;
			}
		}
		{ // slider
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiSliderBarComponent>().value = 0.7;
		}
		{ // progress bar
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiProgressBarComponent>().progress = 0.2;
			e->value<GuiTextComponent>().value = "progress bar";
		}
		{ // color picker
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiColorPickerComponent>();
			envelopeInScrollbars(e);
		}
		{ // image czech flag label
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiImageComponent>().textureName = HashString("cage-tests/gui/czech.png");
			envelopeInScrollbars(e);
		}
		{ // image english flag button
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiImageComponent>().textureName = HashString("cage-tests/gui/english.png");
		}
		{ // panel
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiPanelComponent>();
			e->value<GuiTextComponent>().value = "caption";
			envelopeInScrollbars(e);
			Entity *f = ents->createUnique();
			f->value<GuiParentComponent>().parent = e->id();
			f->value<GuiLabelComponent>();
			f->value<GuiTextComponent>().value = "hello";
		}
		{ // spoiler
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiSpoilerComponent>();
			e->value<GuiTextComponent>().value = "caption";
			envelopeInScrollbars(e);
			Entity *f = ents->createUnique();
			f->value<GuiParentComponent>().parent = e->id();
			f->value<GuiLabelComponent>();
			f->value<GuiTextComponent>().value = "world";
		}
		{ // text area
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = side;
			p.order = index++;
			e->value<GuiTextAreaComponent>().buffer = &buffer;
			//e->value<GuiTextFormatComponent>().lineSpacing = 2;
		}
	}

	void initialize() override
	{
		guiBasicLayout();

		EntityManager *ents = engineGuiEntities();

		{ // animate option
			Entity *e = ents->create(100);
			e->value<GuiParentComponent>().parent = 2;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "animation";
			static constexpr const char *options[] = {
				"margins",
				"borders",
				"paddings",
				"none",
			};
			for (uint32 i = 0; i < sizeof(options) / sizeof(options[0]); i++)
			{
				Entity *ee = ents->createUnique();
				GuiParentComponent &p = ee->value<GuiParentComponent>();
				p.parent = 100;
				p.order = i;
				ee->value<GuiTextComponent>().value = options[i];
			}
		}
		{ // main
			Entity *panel = ents->get(3);
			panel->value<GuiLayoutLineComponent>();
			panel->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5, 0);
		}
		initializeSide(4, "default skin");
		initializeSide(7, "tooltips skin");
		initializeSide(8, "default skin - texture template");
		ents->get(7)->value<GuiWidgetStateComponent>().skin = GuiSkinTooltips;
		ents->get(8)->value<GuiWidgetStateComponent>().skin = GuiSkinIndex(4);
	}
};

MAIN(GuiTestImpl, "skins")
