#include "gui.h"

#include <vector>
#include <algorithm>

class guiTestImpl : public guiTestClass
{
	std::vector<uint32> widgets;

public:

	void generateList(uint32 parent)
	{
		EntityManager *ents = engineGuiEntities();
		uint32 cnt = randomRange(1u, 3u);
		for (uint32 i = 0; i < cnt; i++)
		{
			Entity *e = ents->createUnique();
			widgets.push_back(e->name());
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = parent;
			p.order = i;
			GuiTextComponent &t = e->value<GuiTextComponent>();
			t.value = Stringizer() + e->name();
		}
	}

	void removeOrphanedWidgets()
	{
		EntityManager *ents = engineGuiEntities();
		widgets.erase(std::remove_if(widgets.begin(), widgets.end(), [&](uint32 n)
		{
			Entity *e = ents->get(n);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			bool ret = !ents->has(p.parent);
			if (ret)
				e->destroy();
			return ret;
		}), widgets.end());
	}

	void update() override
	{
		EntityManager *ents = engineGuiEntities();

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
				GuiParentComponent &p = e->value<GuiParentComponent>();
				p.parent = 10;
				p.order = e->name();
			}
			switch (randomRange(0, 13))
			{
			case 0:
			{
				GuiButtonComponent &b = e->value<GuiButtonComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + e->name();
			} break;
			case 1:
			{
				GuiCheckBoxComponent &cb = e->value<GuiCheckBoxComponent>();
				cb.state = CheckBoxStateEnum::Indeterminate;
			} break;
			case 2:
			{
				GuiColorPickerComponent &cp = e->value<GuiColorPickerComponent>();
				cp.color = Vec3(randomRange3(0, 1));
			} break;
			case 3:
			{
				GuiComboBoxComponent &cb = e->value<GuiComboBoxComponent>();
				generateList(e->name());
			} break;
			case 4:
			{
				GuiInputComponent &i = e->value<GuiInputComponent>();
				i.value = Stringizer() + e->name();
			} break;
			case 5:
			{
				GuiLabelComponent &l = e->value<GuiLabelComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + e->name();
			} break;
			case 6:
			{
				GuiListBoxComponent &lb = e->value<GuiListBoxComponent>();
				generateList(e->name());
			} break;
			case 7:
			{
				GuiPanelComponent &p = e->value<GuiPanelComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + e->name();
			} break;
			case 8:
			{
				GuiProgressBarComponent &pb = e->value<GuiProgressBarComponent>();
				pb.progress = randomChance();
				pb.showValue = !!(e->name() % 2);
			} break;
			case 9:
			{
				GuiRadioBoxComponent &rb = e->value<GuiRadioBoxComponent>();
			} break;
			case 10:
			{
				GuiSliderBarComponent &sb = e->value<GuiSliderBarComponent>();
				sb.value = randomChance();
			} break;
			case 11:
			{
				GuiSpoilerComponent &s = e->value<GuiSpoilerComponent>();
				GuiTextComponent &t = e->value<GuiTextComponent>();
				t.value = Stringizer() + e->name();
			} break;
			case 12:
			{
				GuiTextAreaComponent &ta = e->value<GuiTextAreaComponent>();
			} break;
			}
		}
	}

	void initialize() override
	{
		guiBasicLayout();

		EntityManager *ents = engineGuiEntities();

		{
			Entity *e = ents->create(10);
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			GuiLayoutLineComponent &ll = e->value<GuiLayoutLineComponent>();
			ll.vertical = true;
		}
	}

};

MAIN(guiTestImpl, "dynamicChanges")
