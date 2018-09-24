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
	entityManagerClass *ents = gui()->entities();
	GUI_GET_COMPONENT(layoutSplitter, s, ents->get(42));
	switch (e)
	{
	case 1:
	{
		GUI_GET_COMPONENT(checkBox, b, ents->get(e));
		s.allowMasterResize = b.state == checkBoxStateEnum::Checked;
	} break;
	case 2:
	{
		GUI_GET_COMPONENT(checkBox, b, ents->get(e));
		s.allowSlaveResize = b.state == checkBoxStateEnum::Checked;
	} break;
	case 3:
	{
		GUI_GET_COMPONENT(checkBox, b, ents->get(e));
		s.vertical = b.state == checkBoxStateEnum::Checked;
	} break;
	case 4:
	{
		GUI_GET_COMPONENT(checkBox, b, ents->get(e));
		s.inverse = b.state == checkBoxStateEnum::Checked;
	} break;
	case 5:
	{
		GUI_GET_COMPONENT(sliderBar, b, ents->get(e));
		s.anchor = b.value;
	} break;
	case 6:
	{
		GUI_GET_COMPONENT(checkBox, b, ents->get(e));
		GUI_GET_COMPONENT(parent, p1, ents->get(41));
		GUI_GET_COMPONENT(parent, p2, ents->get(42));
		if (b.state == checkBoxStateEnum::Checked)
		{
			p1.parent = 39;
			p2.parent = 40;
		}
		else
		{
			p1.parent = 40;
			p2.parent = 39;
		}
	} break;
	}
	return false;
}

void genContent(sint32 order, const string &name)
{
	entityManagerClass *ents = gui()->entities();
	entityClass *e = ents->createUnique();
	{
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = 42;
		p.order = order;
		GUI_GET_COMPONENT(groupBox, b, e);
		b.type = groupBoxTypeEnum::Spoiler;
		GUI_GET_COMPONENT(text, t, e);
		t.value = name;
		GUI_GET_COMPONENT(layoutLine, l, e);
	}
	entityClass *b = ents->create(100 + order);
	{
		GUI_GET_COMPONENT(parent, p, b);
		p.parent = e->name();
		GUI_GET_COMPONENT(button, but, b);
		GUI_GET_COMPONENT(text, t, b);
		t.value = "Button";
	}
}

void guiInitialize()
{
	entityManagerClass *ents = gui()->entities();

	entityClass *mainSplitter = ents->create(39);
	{
		GUI_GET_COMPONENT(position, p, mainSplitter);
		p.size.value = vec2(1, 1);
		p.size.units[0] = unitEnum::ScreenWidth;
		p.size.units[1] = unitEnum::ScreenHeight;
		GUI_GET_COMPONENT(layoutSplitter, l, mainSplitter);
		l.vertical = true;
	}

	entityClass *menu = ents->createUnique();
	{
		GUI_GET_COMPONENT(parent, p, menu);
		p.parent = mainSplitter->name();
		p.order = 1;
		GUI_GET_COMPONENT(groupBox, b, menu);
		GUI_GET_COMPONENT(layoutLine, l, menu);
	}

	{ // allowMasterResize
		entityClass *e = ents->create(1);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 1;
		GUI_GET_COMPONENT(checkBox, b, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "allowMasterResize";
	}

	{ // allowSlaveResize
		entityClass *e = ents->create(2);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 2;
		GUI_GET_COMPONENT(checkBox, b, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "allowSlaveResize";
	}

	{ // vertical
		entityClass *e = ents->create(3);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 3;
		GUI_GET_COMPONENT(checkBox, b, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "vertical";
	}

	{ // inverse
		entityClass *e = ents->create(4);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 4;
		GUI_GET_COMPONENT(checkBox, b, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "inverse";
	}

	{ // anchor
		entityClass *e = ents->create(5);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 5;
		GUI_GET_COMPONENT(sliderBar, b, e);
	}

	{ // detached
		entityClass *e = ents->create(6);
		GUI_GET_COMPONENT(parent, p, e);
		p.parent = menu->name();
		p.order = 6;
		GUI_GET_COMPONENT(checkBox, b, e);
		GUI_GET_COMPONENT(text, t, e);
		t.value = "detached";
	}

	// center
	entityClass *center = ents->create(40);
	{
		GUI_GET_COMPONENT(position, p, center);
		p.anchor = vec2(0.5, 0.5);
		p.position.value = vec2(0.5, 0.5);
		p.position.units[0] = unitEnum::ScreenWidth;
		p.position.units[1] = unitEnum::ScreenHeight;
		GUI_GET_COMPONENT(layoutLine, l, center);
	}

	// dummy
	entityClass *dummy = ents->create(41);
	{
		GUI_GET_COMPONENT(parent, p, dummy);
		p.parent = 40;
		p.order = 2;
		GUI_GET_COMPONENT(groupBox, b, dummy);
		b.type = groupBoxTypeEnum::Invisible;
	}

	// the test splitter
	entityClass *presentation = ents->create(42);
	{
		GUI_GET_COMPONENT(parent, p, presentation);
		p.parent = 39;
		p.order = 2;
		GUI_GET_COMPONENT(groupBox, b, presentation);
		GUI_GET_COMPONENT(layoutSplitter, l, presentation);
	}

	genContent(1, "First Panel");
	genContent(2, "Second Panel");
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
		window()->title("cage test gui splitter");

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
