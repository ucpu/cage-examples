#include "gui.h"

#include <vector>
#include <algorithm>

class guiTestImpl : public guiTestClass
{
	std::vector<uint32> widgets;

public:

	void generateList(uint32 parent)
	{
		entityManagerClass *ents = gui()->entities();
		uint32 cnt = randomRange(1u, 3u);
		for (uint32 i = 0; i < cnt; i++)
		{
			entityClass *e = ents->createUnique();
			widgets.push_back(e->name());
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = parent;
			p.order = i;
			GUI_GET_COMPONENT(text, t, e);
			t.value = e->name();
		}
	}

	void removeOrphanedWidgets()
	{
		entityManagerClass *ents = gui()->entities();
		widgets.erase(std::remove_if(widgets.begin(), widgets.end(), [&](uint32 n)
		{
			entityClass *e = ents->get(n);
			GUI_GET_COMPONENT(parent, p, e);
			bool ret = !ents->has(p.parent);
			if (ret)
				e->destroy();
			return ret;
		}), widgets.end());
	}

	void update() override
	{
		entityManagerClass *ents = gui()->entities();

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
			entityClass *e = ents->createUnique();
			widgets.push_back(e->name());
			{
				GUI_GET_COMPONENT(parent, p, e);
				p.parent = 10;
				p.order = e->name();
			}
			switch (randomRange(0, 13))
			{
			case 0:
			{
				GUI_GET_COMPONENT(button, b, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = e->name();
			} break;
			case 1:
			{
				GUI_GET_COMPONENT(checkBox, cb, e);
				cb.state = checkBoxStateEnum::Indeterminate;
			} break;
			case 2:
			{
				GUI_GET_COMPONENT(colorPicker, cp, e);
				cp.color = vec3(randomRange3(0, 1));
			} break;
			case 3:
			{
				GUI_GET_COMPONENT(comboBox, cb, e);
				generateList(e->name());
			} break;
			case 4:
			{
				GUI_GET_COMPONENT(input, i, e);
				i.value = e->name();
			} break;
			case 5:
			{
				GUI_GET_COMPONENT(label, l, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = e->name();
			} break;
			case 6:
			{
				GUI_GET_COMPONENT(listBox, lb, e);
				generateList(e->name());
			} break;
			case 7:
			{
				GUI_GET_COMPONENT(panel, p, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = e->name();
			} break;
			case 8:
			{
				GUI_GET_COMPONENT(progressBar, pb, e);
				pb.progress = randomChance();
				pb.showValue = !!(e->name() % 2);
			} break;
			case 9:
			{
				GUI_GET_COMPONENT(radioBox, rb, e);
			} break;
			case 10:
			{
				GUI_GET_COMPONENT(sliderBar, sb, e);
				sb.value = randomChance();
			} break;
			case 11:
			{
				GUI_GET_COMPONENT(spoiler, s, e);
				GUI_GET_COMPONENT(text, t, e);
				t.value = e->name();
			} break;
			case 12:
			{
				GUI_GET_COMPONENT(textArea, ta, e);
			} break;
			}
		}
	}

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();

		{
			entityClass *e = ents->create(10);
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			GUI_GET_COMPONENT(layoutLine, ll, e);
			ll.vertical = true;
		}
	}

};

MAIN(guiTestImpl, "dynamicChanges")
