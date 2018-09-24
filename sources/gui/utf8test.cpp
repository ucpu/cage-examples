#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-core/assets.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;
static const uint32 assetsName = hashString("cage-tests/gui/gui.pack");

// *************************************************************************
// this program may fail due to the text containing malformed utf-8 text
// it is NOT AN ISSUE if the program throws "invalid utf8 string" exception
// in debug build
// *************************************************************************

bool windowClose()
{
	engineStop();
	return false;
}

void guiInitialize()
{
	entityManagerClass *ents = gui()->entities();

	entityClass *panel = ents->create(2);
	{
		GUI_GET_COMPONENT(groupBox, gp, panel);
		GUI_GET_COMPONENT(position, pos, panel);
		pos.size.units[0] = unitEnum::ScreenWidth;
		pos.size.units[1] = unitEnum::ScreenHeight;
		pos.size.value = vec2(1, 1);
		GUI_GET_COMPONENT(layoutLine, ll, panel);
		ll.vertical = true;
	}

	for (uint32 i = 0; i < 300; i++)
	{
		entityClass * e = ents->create(100 + i);
		GUI_GET_COMPONENT(parent, parent, e);
		parent.parent = panel->name();
		parent.order = i;
		GUI_GET_COMPONENT(label, label, e);
		GUI_GET_COMPONENT(text, text, e);
		text.assetName = hashString("cage-tests/gui/utf8test.textpack");
		text.textName = hashString((string() + "utf8test/" + i).c_str());
		GUI_GET_COMPONENT(textFormat, format, e);
		format.font = hashString("cage-tests/gui/DroidSansMono.ttf");
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
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test utf8");

		guiInitialize();
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
