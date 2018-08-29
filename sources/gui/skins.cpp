#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/assets.h>
#include <cage-core/utility/hashString.h>
#include <cage-core/utility/color.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/utility/highPerformanceGpuHint.h>

using namespace cage;

bool windowClose()
{
	engineStop();
	return false;
}

real steeper(rads x)
{
	return (9 * sin(x) + sin(x * 3)) / 8;
}

bool update()
{
	entityManagerClass *ents = gui()->entities();

	uint32 animateOption = -1;
	{
		GUI_GET_COMPONENT(comboBox, combo, ents->getEntity(100));
		animateOption = combo.selected;
	}

	static real offset = random() * 1000;
	real t = getApplicationTime() / 5e6f + offset;

	vec4 a4;
	for (uint32 i = 0; i < 4; i++)
		a4[i] = (steeper(rads(t) + rads::Full * real(i) / 4) * 0.5 + 0.5) * 20;

	skinConfigStruct skin;
	switch (animateOption)
	{
	case 0: // margins
		skin.defaults.button.margin = a4;
		skin.defaults.checkBox.margin = a4;
		skin.defaults.colorPicker.margin = a4;
		skin.defaults.comboBox.baseMargin = a4;
		skin.defaults.groupBox.baseMargin = a4;
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
		skin.defaults.groupBox.captionPadding = a4;
		skin.defaults.groupBox.contentPadding = a4;
		skin.defaults.inputBox.basePadding = a4;
		skin.defaults.sliderBar.horizontal.padding = a4;
		break;
	}
	gui()->skin(0) = gui()->skin(1) = skin;
	gui()->skin(1).textureName = hashString("cage-tests/gui/skin.png");

	return false;
}

void guiInitialize()
{
	entityManagerClass *ents = gui()->entities();

	{ // splitter
		entityClass *split = ents->newEntity(1);
		GUI_GET_COMPONENT(layoutSplitter, ls, split);
		ls.vertical = true;
		ls.anchor = 0.5;
		ls.allowSlaveResize = true;
		GUI_GET_COMPONENT(position, ep, split);
		ep.size.value = vec2(1, 1);
		ep.size.units[0] = unitEnum::ScreenWidth;
		ep.size.units[1] = unitEnum::ScreenHeight;
	}
	{ // top panel
		entityClass *panel = ents->newEntity(2);
		GUI_GET_COMPONENT(parent, p, panel);
		p.parent = 1;
		p.order = 1;
		GUI_GET_COMPONENT(groupBox, gp, panel);
	}
	{ // animate option
		entityClass *e = ents->newEntity(100);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = 2;
		GUI_GET_COMPONENT(comboBox, input, e);
		GUI_GET_COMPONENT(text, text, e);
		text.value = "animation";
		static const char *options[] = {
			"margins",
			"borders",
			"paddings",
		};
		for (uint32 i = 0; i < sizeof(options)/sizeof(options[0]); i++)
		{
			entityClass *ee = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, ee);
			p.parent = 100;
			p.order = i;
			GUI_GET_COMPONENT(text, text, ee);
			text.value = options[i];
		}
	}
	{ // bottom
		entityClass *panel = ents->newEntity(3);
		GUI_GET_COMPONENT(parent, p, panel);
		p.parent = 1;
		p.order = 2;
		GUI_GET_COMPONENT(layoutLine, ll, panel);
	}
	{ // left panel
		entityClass *panel = ents->newEntity(4);
		GUI_GET_COMPONENT(parent, p, panel);
		p.parent = 3;
		p.order = 1;
		GUI_GET_COMPONENT(groupBox, gp, panel);
		GUI_GET_COMPONENT(layoutLine, ll, panel);
		ll.vertical = true;
	}
	{ // right panel
		entityClass *panel = ents->newEntity(5);
		GUI_GET_COMPONENT(parent, p, panel);
		p.parent = 3;
		p.order = 2;
		GUI_GET_COMPONENT(groupBox, gp, panel);
		GUI_GET_COMPONENT(layoutLine, ll, panel);
		ll.vertical = true;
		GUI_GET_COMPONENT(widgetState, ws, panel);
		ws.skinIndex = 1;
	}
	for (uint32 side = 4; side < 6; side++)
	{
		uint32 index = 0;
		{ // label
			entityClass *e = ents->newUniqueEntity();
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
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(button, button, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "button";
		}
		{ // text input box
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(inputBox, input, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "text input box";
		}
		{ // real input box
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(inputBox, input, e);
			input.type = inputTypeEnum::Real;
			GUI_GET_COMPONENT(text, text, e);
			text.value = "real input box";
		}
		{ // checkbox
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(checkBox, box, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "checkbox";
		}
		{ // combo box
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(comboBox, input, e);
			GUI_GET_COMPONENT(text, text, e);
			text.value = "combo box";
			for (uint32 i = 0; i < 5; i++)
			{
				entityClass *ee = ents->newUniqueEntity();
				GUI_GET_COMPONENT(parent, p, ee);
				p.parent = e->getName();
				p.order = i;
				GUI_GET_COMPONENT(text, text, ee);
				text.value = i;
			}
		}
		{ // slider
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(sliderBar, slider, e);
		}
		{ // color picker
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = side;
			p.order = index++;
			GUI_GET_COMPONENT(colorPicker, picker, e);
		}
	}
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		{
			guiCreateConfig g;
			g.skinsCount = 2;
			engineCreateConfig e;
			e.gui = &g;
			engineInitialize(e);
		}

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test gui skins");

		guiInitialize();
		static const uint32 assetsName = hashString("cage-tests/gui/gui.pack");
		assets()->add(assetsName);
		engineStart();
		assets()->remove(assetsName);
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
