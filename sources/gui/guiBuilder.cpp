#include <cage-engine/guiBuilder.h>
#include <cage-core/memoryBuffer.h>
#include "gui.h"

class GuiTestImpl : public GuiTestClass
{
public:
	MemoryBuffer buffer;

	bool switchSkin(uint32 comboBox)
	{
		EntityManager *ents = engineGuiEntities();
		ents->get(3)->value<GuiWidgetStateComponent>().skinIndex = ents->get(comboBox)->value<GuiComboBoxComponent>().selected;
		return true;
	}

	void initialize() override
	{
		guiBasicLayout();

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(2));
			auto _ = g->comboBox().text("skins").event(Delegate<bool(uint32)>().bind<GuiTestImpl, &GuiTestImpl::switchSkin>(this));
			static constexpr const char *options[] = {
				"default",
				"large",
				"compact",
				"tooltips",
			};
			for (uint32 i = 0; i < sizeof(options) / sizeof(options[0]); i++)
				g->empty().text(options[i]);
		}

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(3));
			auto _ = g->verticalTable();

			for (uint32 i = 0; i < 4; i++)
			{
				g->label().text("label");
				g->label().text(Stringizer() + "text " + i);
			}

			g->label().text("default input");
			g->input("");

			g->label().text("check box");
			g->checkBox().text("label");

			g->label().text("radio box");
			g->radioBox().text("label");

			g->label().text("combo box");
			{
				auto _ = g->comboBox().text("select one:");
				for (uint32 i = 0; i < 4; i++)
					g->empty().text(Stringizer() + "option " + i);
			}

			g->label().text("multiline");
			g->label().text("lorem ipsum dolor sit amet\nthe quick brown fox jumps over the lazy dog\n1 + 1 = 3\n\nlast line");

			g->label().text("color picker");
			g->colorPicker(Vec3(1, 0, 0), true);

			g->label().text("button 1");
			g->button().text("text");

			g->label().text("button 2");
			g->button().size(Vec2(120)).image(GuiImageComponent{ .textureUvOffset = Vec2(5 / 8.f, 2 / 8.f), .textureUvSize = Vec2(1 / 8.f, 1 / 8.f), .textureName = HashString("cage/texture/helper.jpg") });

			g->label().text("slider");
			g->horizontalSliderBar(0.3);

			g->label().text("progress bar");
			g->progressBar(0.2).text("loading");

			g->label().text("integer input");
			g->input(0, -13, 42, 5);

			g->label().text("disabled input");
			g->input(0, -13, 42, 5).disabled();

			g->label().text("text area");
			g->textArea(&buffer);
		}
	}
};

MAIN(GuiTestImpl, "gui builder")