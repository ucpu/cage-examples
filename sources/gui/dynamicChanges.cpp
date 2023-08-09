#include <algorithm>
#include <vector>

#include "gui.h"

#include <cage-core/memoryBuffer.h>

using namespace cage;

class GuiTestImpl : public GuiTestClass
{
	std::vector<uint32> widgets;
	MemoryBuffer buffer;

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
			e->value<GuiTextComponent>().value = Stringizer() + e->name();
		}
	}

	void removeOrphanedWidgets()
	{
		EntityManager *ents = engineGuiEntities();
		widgets.erase(std::remove_if(widgets.begin(), widgets.end(),
						  [&](uint32 n)
						  {
							  Entity *e = ents->get(n);
							  GuiParentComponent &p = e->value<GuiParentComponent>();
							  bool ret = !ents->has(p.parent);
							  if (ret)
								  e->destroy();
							  return ret;
						  }),
			widgets.end());
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
			switch (randomRange(0, 12))
			{
				case 0:
				{
					e->value<GuiButtonComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + e->name();
				}
				break;
				case 1:
				{
					e->value<GuiCheckBoxComponent>().state = CheckBoxStateEnum::Indeterminate;
				}
				break;
				case 2:
				{
					e->value<GuiColorPickerComponent>().color = randomChance3();
				}
				break;
				case 3:
				{
					e->value<GuiComboBoxComponent>();
					generateList(e->name());
				}
				break;
				case 4:
				{
					e->value<GuiInputComponent>().value = Stringizer() + e->name();
				}
				break;
				case 5:
				{
					e->value<GuiLabelComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + e->name();
				}
				break;
				case 6:
				{
					e->value<GuiPanelComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + e->name();
				}
				break;
				case 7:
				{
					GuiProgressBarComponent &pb = e->value<GuiProgressBarComponent>();
					pb.progress = randomChance();
					pb.showValue = randomChance() < 0.5;
				}
				break;
				case 8:
				{
					e->value<GuiRadioBoxComponent>();
				}
				break;
				case 9:
				{
					e->value<GuiSliderBarComponent>().value = randomChance();
				}
				break;
				case 10:
				{
					e->value<GuiSpoilerComponent>();
					e->value<GuiTextComponent>().value = Stringizer() + e->name();
				}
				break;
				case 11:
				{
					e->value<GuiTextAreaComponent>().buffer = &buffer;
				}
				break;
			}
		}
	}

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		{
			Entity *e = ents->create(10);
			e->value<GuiPanelComponent>();
			e->value<GuiLayoutAlignmentComponent>().alignment = Vec2(0.5, 0);
			e->value<GuiLayoutLineComponent>().vertical = true;
		}
	}
};

MAIN(GuiTestImpl, "dynamic changes")
