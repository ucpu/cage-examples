#include "gui.h"

#include <cage-core/memoryBuffer.h>
#include <cage-engine/guiBuilder.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
public:
	MemoryBuffer buffer;
	uint32 updatedIndex = 0;

	bool switchSkin(input::GuiValue in)
	{
		EntityManager *ents = engineGuiEntities();
		ents->get(3)->value<GuiWidgetStateComponent>().skin = GuiSkinIndex(in.entity->value<GuiComboBoxComponent>().selected);
		return true;
	}

	void initialize() override
	{
		guiBasicLayout();

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(2));
			auto _ = g->comboBox().text("skins").event(inputFilter([this](input::GuiValue in) { switchSkin(in); }));
			static constexpr const char *options[] = {
				"default",
				"large",
				"compact",
				"tooltips",
			};
			for (uint32 i = 0; i < array_size(options); i++)
				g->empty().text(options[i]);
		}

		{
			Holder<GuiBuilder> g = newGuiBuilder(engineGuiEntities()->get(3));
			auto _ = g->verticalTable();

			g->label().text("header");
			g->header().text("Header");

			for (uint32 i = 0; i < 4; i++)
			{
				g->label().text("label");
				g->label().text(Stringizer() + "text " + i);
			}

			g->label().text("header 2");
			g->header().text("Header 2");

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

			g->label().text("solid color");
			g->solidColor(Vec3(0, 1, 0));

			g->label().text("button 1");
			g->button().text("text");

			g->label().text("button 2");
			g->button().size(Vec2(120)).image(GuiImageComponent{ .textureUvOffset = Vec2(5 / 8.f, 2 / 8.f), .textureUvSize = Vec2(1 / 8.f, 1 / 8.f), .textureName = HashString("cage/textures/helper.jpg") });

			g->label().text("separator");
			g->horizontalSeparator();

			g->label().text("slider");
			g->horizontalSliderBar(0.3);

			g->label().text("progress bar");
			g->progressBar(0.2).text("loading");

			g->label().text("integer input");
			g->input(0, -13, 42, 5);

			g->label().text("disabled input");
			g->input(0, -13, 42, 5).disabled();

			g->label().text("spoiler");
			{
				auto _1 = g->spoiler().text("Quotes");
				auto _2 = g->column();
				g->label().text("Be yourself; everyone else is already taken.");
				g->label().text("Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.");
				g->label().text("The fool doth think he is wise, but the wise man knows himself to be a fool.");
				g->label().text("The opposite of love is not hate, it's indifference. The opposite of art is not ugliness, it's indifference.\nThe opposite of faith is not heresy, it's indifference. And the opposite of life is not death, it's indifference.");
				g->label().text("The man who does not read has no advantage over the man who cannot read.");
				g->label().text("I solemnly swear that I am up to no good.");
			}

			g->label().text("text area");
			g->textArea(&buffer);

			g->label().text("updated label");
			g->label().text("").update([&](Entity *e) { e->value<GuiTextComponent>().value = Stringizer() + updatedIndex++; });

			g->label().text("red accent");
			{
				auto _ = g->row();
				for (Real a = 0.1; a < 0.41; a += 0.1)
					g->button().text(Stringizer() + a).accent(Vec4(1, 0, 0, a));
			}

			g->label().text("green accent");
			{
				auto _ = g->row();
				for (Real a = 0.1; a < 0.41; a += 0.1)
					g->button().text(Stringizer() + a).accent(Vec4(0, 1, 0, a));
			}

			g->label().text("blue accent");
			{
				auto _ = g->row();
				for (Real a = 0.1; a < 0.41; a += 0.1)
					g->button().text(Stringizer() + a).accent(Vec4(0, 0, 1, a));
			}

			g->label().text("frame");
			{
				auto _1 = g->frame();
				auto _2 = g->row();
				g->label().text("aaa");
				g->label().text("bbb");
			}
		}
	}
};

MAIN(GuiTestImpl, "gui builder")
