#include "gui.h"

#include <cage-core/memoryBuffer.h>
#include <cage-engine/guiSkins.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	MemoryBuffer buffer;

	bool switchSkin(input::GuiValue in)
	{
		EntityManager *ents = engineGuiEntities();
		ents->get(3)->value<GuiWidgetStateComponent>().skin = GuiSkinIndex(in.entity->value<GuiComboBoxComponent>().selected);
		return true;
	}

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->value<GuiLayoutTableComponent>();
		}

		{ // skin selection
			Entity *e = ents->create(1000);
			e->value<GuiParentComponent>().parent = 2;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "skins";
			e->value<GuiEventComponent>().event = inputFilter([this](input::GuiValue in) { switchSkin(in); });
			static constexpr const char *options[] = {
				"default",
				"large",
				"compact",
				"tooltips",
			};
			for (uint32 i = 0; i < sizeof(options) / sizeof(options[0]); i++)
			{
				Entity *ee = ents->createUnique();
				GuiParentComponent &p = ee->value<GuiParentComponent>();
				p.parent = 1000;
				p.order = i;
				ee->value<GuiTextComponent>().value = options[i];
			}
		}

		uint32 index = 1;

		for (uint32 i = 0; i < 4; i++)
		{ // labels
			guiLabel(3, index, "labels");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = Stringizer() + "text " + i;
		}

		{ // default input
			guiLabel(3, index, "default input");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiInputComponent>();
		}

		{ // check box
			guiLabel(3, index, "check box");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiCheckBoxComponent>();
			e->value<GuiTextComponent>().value = "label";
		}

		{ // radio box
			guiLabel(3, index, "radio box");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiRadioBoxComponent>();
			e->value<GuiTextComponent>().value = "label";
		}

		{ // combo box
			guiLabel(3, index, "combo box");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiComboBoxComponent>();
			e->value<GuiTextComponent>().value = "select one:";
			for (uint32 i = 0; i < 4; i++)
			{
				Entity *o = ents->createUnique();
				GuiParentComponent &p = o->value<GuiParentComponent>();
				p.parent = e->id();
				p.order = index++;
				o->value<GuiTextComponent>().value = Stringizer() + "option " + i;
			}
		}

		{ // multiline
			guiLabel(3, index, "multiline");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiLabelComponent>();
			e->value<GuiTextComponent>().value = "lorem ipsum dolor sit amet\nthe quick brown fox jumps over the lazy dog\n1 + 1 = 3\n\nlast line";
		}

		{ // color picker
			guiLabel(3, index, "color picker");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiColorPickerComponent>().collapsible = true;
		}

		{ // button 1
			guiLabel(3, index, "button 1");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			e->value<GuiTextComponent>().value = "text";
		}

		{ // button 2
			guiLabel(3, index, "button 2");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiButtonComponent>();
			GuiImageComponent &m = e->value<GuiImageComponent>();
			m.textureName = HashString("cage/texture/helper.jpg");
			m.textureUvOffset = Vec2(5 / 8.f, 2 / 8.f);
			m.textureUvSize = Vec2(1 / 8.f, 1 / 8.f);
			e->value<GuiExplicitSizeComponent>().size = Vec2(120, 120);
		}

		{ // slider
			guiLabel(3, index, "slider");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiSliderBarComponent>().value = 0.3;
		}

		{ // progress bar
			guiLabel(3, index, "progress bar");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiProgressBarComponent>().progress = 0.2;
			e->value<GuiTextComponent>().value = "loading";
		}

		{ // integer input
			guiLabel(3, index, "integer input");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiInputComponent &ib = e->value<GuiInputComponent>();
			ib.type = InputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
		}

		{ // disabled input
			guiLabel(3, index, "disabled input");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			GuiInputComponent &ib = e->value<GuiInputComponent>();
			ib.type = InputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
			e->value<GuiWidgetStateComponent>().disabled = true;
		}

		{ // text area
			guiLabel(3, index, "text area");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiTextAreaComponent>().buffer = &buffer;
		}
	}
};

MAIN(GuiTestImpl, "mixture")
