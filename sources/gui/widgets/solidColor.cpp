#include "../gui.h"

#include <cage-core/color.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
	void initialize() override
	{
		guiBasicLayout();
		auto g = newGuiBuilder(engineGuiEntities()->get(3));
		auto _ = g->topColumn();
		{
			g->label().text("gui solid colors").textAlign(TextAlignEnum::Center);
			auto _ = g->horizontalTable(3);
			{
				g->label().text("value:");
				g->label().text("color:");
				g->label().text("linear to gamma:");
			}
			for (float v = 0; v <= 1; v += 0.03)
			{
				g->label().text(Stringizer() + int(v * 255));
				g->solidColor(Vec3(v));
				g->solidColor(colorLinearToGamma(Vec3(v)));
			}
		}
		{
			g->label().text("reference picture").textAlign(TextAlignEnum::Center);
			auto _ = g->alignment(Vec2(0.5, 0));
			g->label().image(HashString("cage-tests/gui/linearVsGamma.png")).size(Vec2(2400, 612) * 0.5);
		}
		{
			g->label().text("color picker").textAlign(TextAlignEnum::Center);
			auto _ = g->centerRow();
			const uint32 cpid = g->colorPicker()->id();
			g->solidColor().update([cpid](Entity *e) { e->value<GuiSolidColorComponent>().color = engineGuiEntities()->get(cpid)->value<GuiColorPickerComponent>().color; }).size(Vec2(150));
		}
		{
			g->label().text("color palette").textAlign(TextAlignEnum::Center);
			{
				auto _ = g->alignment();
				g->label().image(HashString("cage-tests/gui/colorPalette.png")).size(Vec2(800, 452) * 0.5);
			}
			{
				auto _ = g->centerRow();
				g->solidColor(Vec3(212, 174, 56) / 255).size(Vec2(160) * 0.5);
				g->solidColor(Vec3(11, 44, 92) / 255).size(Vec2(160) * 0.5);
				g->solidColor(Vec3(21, 108, 118) / 255).size(Vec2(160) * 0.5);
				g->solidColor(Vec3(47, 138, 88) / 255).size(Vec2(160) * 0.5);
				g->solidColor(Vec3(243, 239, 231) / 255).size(Vec2(160) * 0.5);
			}
		}
		{
			g->label().text("gui transparent colors").textAlign(TextAlignEnum::Center);
			auto _ = g->horizontalTable(2);
			{
				g->label().text("value:");
				g->label().text("color:");
			}
			for (float v = 0; v <= 1; v += 0.05)
			{
				g->label().text(Stringizer() + int(v * 100) + " %");
				g->solidColor(Vec3(212, 174, 56) / 255, v);
			}
		}
	}
};

MAIN(GuiTestImpl, "solid colors")
