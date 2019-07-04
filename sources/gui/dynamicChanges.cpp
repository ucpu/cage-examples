#include "gui.h"

#include <vector>
#include <algorithm>

class guiTestImpl : public guiTestClass
{
	std::vector<uint32> widgets;

public:

	void generateList(uint32 parent)
	{
		entityManager *ents = gui()->entities();
		uint32 cnt = randomRange(1u, 3u);
		for (uint32 i = 0; i < cnt; i++)
		{
			entity *e = ents->createUnique();
			widgets.push_back(e->name());
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = parent;
			p.order = i;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = e->name();
		}
	}

	void removeOrphanedWidgets()
	{
		entityManager *ents = gui()->entities();
		widgets.erase(std::remove_if(widgets.begin(), widgets.end(), [&](uint32 n)
		{
			entity *e = ents->get(n);
			CAGE_COMPONENT_GUI(parent, p, e);
			bool ret = !ents->has(p.parent);
			if (ret)
				e->destroy();
			return ret;
		}), widgets.end());
	}

	void update() override
	{
		entityManager *ents = gui()->entities();

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
			entity *e = ents->createUnique();
			widgets.push_back(e->name());
			{
				CAGE_COMPONENT_GUI(parent, p, e);
				p.parent = 10;
				p.order = e->name();
			}
			switch (randomRange(0, 13))
			{
			case 0:
			{
				CAGE_COMPONENT_GUI(button, b, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = e->name();
			} break;
			case 1:
			{
				CAGE_COMPONENT_GUI(checkBox, cb, e);
				cb.state = checkBoxStateEnum::Indeterminate;
			} break;
			case 2:
			{
				CAGE_COMPONENT_GUI(colorPicker, cp, e);
				cp.color = vec3(randomRange3(0, 1));
			} break;
			case 3:
			{
				CAGE_COMPONENT_GUI(comboBox, cb, e);
				generateList(e->name());
			} break;
			case 4:
			{
				CAGE_COMPONENT_GUI(input, i, e);
				i.value = e->name();
			} break;
			case 5:
			{
				CAGE_COMPONENT_GUI(label, l, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = e->name();
			} break;
			case 6:
			{
				CAGE_COMPONENT_GUI(listBox, lb, e);
				generateList(e->name());
			} break;
			case 7:
			{
				CAGE_COMPONENT_GUI(panel, p, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = e->name();
			} break;
			case 8:
			{
				CAGE_COMPONENT_GUI(progressBar, pb, e);
				pb.progress = randomChance();
				pb.showValue = !!(e->name() % 2);
			} break;
			case 9:
			{
				CAGE_COMPONENT_GUI(radioBox, rb, e);
			} break;
			case 10:
			{
				CAGE_COMPONENT_GUI(sliderBar, sb, e);
				sb.value = randomChance();
			} break;
			case 11:
			{
				CAGE_COMPONENT_GUI(spoiler, s, e);
				CAGE_COMPONENT_GUI(text, t, e);
				t.value = e->name();
			} break;
			case 12:
			{
				CAGE_COMPONENT_GUI(textArea, ta, e);
			} break;
			}
		}
	}

	void initialize() override
	{
		guiBasicLayout();

		entityManager *ents = gui()->entities();

		{
			entity *e = ents->create(10);
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			CAGE_COMPONENT_GUI(layoutLine, ll, e);
			ll.vertical = true;
		}
	}

};

MAIN(guiTestImpl, "dynamicChanges")
