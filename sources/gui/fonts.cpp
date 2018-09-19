#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/assets.h>
#include <cage-core/hashString.h>
#include <cage-core/color.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

const char *fontNames[] = {
	"cage-tests/gui/DroidSansMono.ttf",
	"cage-tests/gui/immortal.ttf",
	"cage-tests/gui/roboto.ttf",
	"cage-tests/gui/asimov.otf",
	"cage/font/ubuntu/Ubuntu-R.ttf",
};
const string labelTexts[] = {
	"Droid Sans Mono",
	"Immortal",
	"Roboto",
	"Asimov",
	"Ubuntu",
};
const uint32 fontsCount = sizeof(fontNames) / sizeof(fontNames[0]);
CAGE_ASSERT_COMPILE(sizeof(fontNames) / sizeof(fontNames[0]) == sizeof(labelTexts) / sizeof(labelTexts[0]), arrays_must_have_same_number_of_elements);

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
	static real offset = random() * 1000;
	real t = getApplicationTime() / 2e7f + offset;
	for (uint32 i = 0; i < fontsCount; i++)
	{
		entityClass * e = ents->getEntity(100 + i);
		GUI_GET_COMPONENT(textFormat, format, e);
		format.size = (steeper(rads(t) + rads::Full * real(i) / fontsCount) * 0.5 + 0.5) * 80 + 10;
	}
	return false;
}

void guiInitialize()
{
	entityManagerClass *ents = gui()->entities();

	entityClass *panel = ents->newEntity(2);
	{
		GUI_GET_COMPONENT(groupBox, gp, panel);
		GUI_GET_COMPONENT(position, pos, panel);
		pos.anchor = vec2(.5, .5);
		pos.position.units[0] = unitEnum::ScreenWidth;
		pos.position.units[1] = unitEnum::ScreenHeight;
		pos.position.value = vec2(.5, .5);
		GUI_GET_COMPONENT(layoutLine, ll, panel);
		ll.vertical = true;
		ll.cellsAnchor = vec2(.5, .5);
	}

	for (uint32 i = 0; i < fontsCount; i++)
	{
		entityClass * e = ents->newEntity(100 + i);
		GUI_GET_COMPONENT(parent, parent, e);
		parent.parent = panel->getName();
		parent.order = i;
		GUI_GET_COMPONENT(label, label, e);
		GUI_GET_COMPONENT(text, text, e);
		text.value = labelTexts[i];
		GUI_GET_COMPONENT(textFormat, format, e);
		format.font = hashString(fontNames[i]);
		format.align = textAlignEnum::Center;
		format.lineSpacing = 1;
		format.color = convertHsvToRgb(vec3(random(), 1, 1));
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

		engineInitialize(engineCreateConfig());

		// events
#define GCHL_GENERATE(TYPE, FUNC, EVENT) eventListener<bool TYPE> CAGE_JOIN(FUNC, Listener); CAGE_JOIN(FUNC, Listener).bind<&FUNC>(); CAGE_JOIN(FUNC, Listener).attach(EVENT);
		GCHL_GENERATE((), windowClose, window()->events.windowClose);
		GCHL_GENERATE((), update, controlThread().update);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test fonts");

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
