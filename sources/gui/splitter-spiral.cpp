#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/entities.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

bool windowClose()
{
	engineStop();
	return false;
}

bool update()
{
	entityManagerClass *ents = entities();
	return false;
}

bool guiEvent(uint32 e)
{
	CAGE_LOG(severityEnum::Info, "gui event", string() + "gui event on entity: " + e);
	return false;
}

entityClass *makeSplitterRec(uint32 depth)
{
	entityManagerClass *ents = gui()->entities();

	entityClass *cell = ents->newUniqueEntity();
	{
		GUI_GET_COMPONENT(groupBox, b, cell);
		b.type = groupBoxTypeEnum::Panel;
	}

	if (depth == 15)
	{
		entityClass *e = ents->newUniqueEntity();
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = cell->getName();
		GUI_GET_COMPONENT(label, l, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "Hi";
	}
	else
	{
		GUI_GET_COMPONENT(layoutSplitter, s, cell);
		s.vertical = (depth % 2) == 1;
		s.inverse = ((depth / 2) % 2) == 1;
		{ // first
			entityClass *e = ents->newUniqueEntity();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = cell->getName();
			p.order = s.inverse;
			GUI_GET_COMPONENT(button, l, e);
			GUI_GET_COMPONENT(text, t, e);
			t.value = string() + "Item: " + (depth + 1);
		}
		{ // second
			entityClass *e = makeSplitterRec(depth + 1);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = cell->getName();
			p.order = !s.inverse;
		}
	}

	return cell;
}

void guiInitialize()
{
	entityManagerClass *ents = gui()->entities();
	entityClass *e = makeSplitterRec(0);
	GUI_GET_COMPONENT(position, p, e);
	p.size.value = vec2(1, 1);
	p.size.units[0] = unitEnum::ScreenWidth;
	p.size.units[1] = unitEnum::ScreenHeight;
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
		GCHL_GENERATE((uint32), guiEvent, gui()->widgetEvent);
#undef GCHL_GENERATE

		// window
		window()->modeSetWindowed((windowFlags)(windowFlags::Border | windowFlags::Resizeable));
		window()->windowedSize(pointStruct(800, 600));
		window()->title("cage test gui splitter spiral");

		guiInitialize();
		engineStart();
		engineFinalize();

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
