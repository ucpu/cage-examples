#include "gui.h"

#include <vector>
#include <algorithm>

class guiTestImpl : public guiTestClass
{
	std::vector<uint32> widgets;

public:

	void generateList(uint32 parent)
	{
		EntityManager *ents = gui()->entities();
		uint32 cnt = randomRange(1u, 3u);
		for (uint32 i = 0; i < cnt; i++)
		{
			Entity *e = ents->createUnique();
			widgets.push_back(e->name());
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = parent;
			p.order = i;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = string(e->name());
		}
	}

	void removeOrphanedWidgets()
	{
		EntityManager *ents = gui()->entities();
		widgets.erase(std::remove_if(widgets.begin(), widgets.end(), [&](uint32 n)
		{
			Entity *e = ents->get(n);
			CAGE_COMPONENT_GUI(Parent, p, e);
			bool ret = !ents->has(p.parent);
			if (ret)
				e->destroy();
			return ret;
		}), widgets.end());
	}

	void update() override
	{
		EntityManager *ents = gui()->entities();

		// remove widget
		if (widgets.size() > 10 || (widgets.size() > 3 && randomChance() < 0.1))
		{
			uint32 pick = randomRange(0u, numeric_cast<uint32>(widgets.size()) - 1u);
			ents->get(widgets[pick])->destroy();
			widgets.erase(widgets.begin() + pick);
			removeOrphanedWidgets();
		}

		// add widget
		if (widgets.size() < 3 || (widgets.size() < 10 && randomChance() < 0.1))
		{
			Entity *e = ents->createUnique();
			widgets.push_back(e->name());
			{
				CAGE_COMPONENT_GUI(Parent, p, e);
				p.parent = 10;
				p.order = e->name();
			}
			switch (randomRange(0, 13))
			{
			case 0:
			{
				CAGE_COMPONENT_GUI(Button, b, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(e->name());
			} break;
			case 1:
			{
				CAGE_COMPONENT_GUI(CheckBox, cb, e);
				cb.state = CheckBoxStateEnum::Indeterminate;
			} break;
			case 2:
			{
				CAGE_COMPONENT_GUI(ColorPicker, cp, e);
				cp.color = vec3(randomRange3(0, 1));
			} break;
			case 3:
			{
				CAGE_COMPONENT_GUI(ComboBox, cb, e);
				generateList(e->name());
			} break;
			case 4:
			{
				CAGE_COMPONENT_GUI(Input, i, e);
				i.value = string(e->name());
			} break;
			case 5:
			{
				CAGE_COMPONENT_GUI(Label, l, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(e->name());
			} break;
			case 6:
			{
				CAGE_COMPONENT_GUI(ListBox, lb, e);
				generateList(e->name());
			} break;
			case 7:
			{
				CAGE_COMPONENT_GUI(Panel, p, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(e->name());
			} break;
			case 8:
			{
				CAGE_COMPONENT_GUI(ProgressBar, pb, e);
				pb.progress = randomChance();
				pb.showValue = !!(e->name() % 2);
			} break;
			case 9:
			{
				CAGE_COMPONENT_GUI(RadioBox, rb, e);
			} break;
			case 10:
			{
				CAGE_COMPONENT_GUI(SliderBar, sb, e);
				sb.value = randomChance();
			} break;
			case 11:
			{
				CAGE_COMPONENT_GUI(Spoiler, s, e);
				CAGE_COMPONENT_GUI(Text, t, e);
				t.value = string(e->name());
			} break;
			case 12:
			{
				CAGE_COMPONENT_GUI(TextArea, ta, e);
			} break;
			}
		}
	}

	void initialize() override
	{
		guiBasicLayout();

		EntityManager *ents = gui()->entities();

		{
			Entity *e = ents->create(10);
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			CAGE_COMPONENT_GUI(LayoutLine, ll, e);
			ll.vertical = true;
		}
	}

};

MAIN(guiTestImpl, "dynamicChanges")
