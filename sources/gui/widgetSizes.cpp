#include "gui.h"

class guiTestImpl : public guiTestClass
{
public:

	template <class T>
	struct widgetScrollbars
	{
		static const bool enable = false;
	};

	template <class T>
	struct widgetVariations
	{
		static const uint32 count = 1;
	};

	template <class T>
	void widgetInit(entityClass *e, uint32 variation)
	{}

	template <>
	void widgetInit<labelComponent>(entityClass *e, uint32 variation)
	{
		GUI_GET_COMPONENT(text, t, e);
		t.value = "Some Text";
	}

	void addItems(entityClass *e)
	{
		entityManagerClass *ents = gui()->entities();

		for (uint32 i = 0; i < 4; i++)
		{
			entityClass *o = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, o);
			p.parent = e->name();
			p.order = i;
			GUI_GET_COMPONENT(text, t, o);
			t.value = string("line ") + i;
		}
	}

	template <>
	void widgetInit<comboBoxComponent>(entityClass *e, uint32 variation)
	{
		addItems(e);
	}

	template <>
	void widgetInit<listBoxComponent>(entityClass *e, uint32 variation)
	{
		addItems(e);
	}

	template <>
	struct widgetVariations<sliderBarComponent>
	{
		static const uint32 count = 2;
	};

	template <>
	void widgetInit<sliderBarComponent>(entityClass *e, uint32 variation)
	{
		GUI_GET_COMPONENT(sliderBar, sb, e);
		sb.vertical = (variation % 2) == 1;
	}

	template <>
	struct widgetVariations<colorPickerComponent>
	{
		static const uint32 count = 2;
	};

	template <>
	void widgetInit<colorPickerComponent>(entityClass *e, uint32 variation)
	{
		GUI_GET_COMPONENT(colorPicker, cp, e);
		cp.collapsible = (variation % 2) == 1;
		cp.color = randomChance3();
	}

	void addContent(entityClass *e)
	{
		GUI_GET_COMPONENT(layoutLine, ll, e);
		ll.vertical = true;

		entityManagerClass *ents = gui()->entities();

		for (uint32 i = 0; i < 4; i++)
		{
			entityClass *o = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, o);
			p.parent = e->name();
			p.order = i;
			GUI_GET_COMPONENT(button, but, o);
			GUI_GET_COMPONENT(text, t, o);
			t.value = string("line ") + i;
		}
	}

	template <>
	struct widgetScrollbars<panelComponent>
	{
		static const bool enable = true;
	};

	template <>
	void widgetInit<panelComponent>(entityClass *e, uint32 variation)
	{
		addContent(e);
	}

	template <>
	struct widgetScrollbars<spoilerComponent>
	{
		static const bool enable = true;
	};

	template <>
	void widgetInit<spoilerComponent>(entityClass *e, uint32 variation)
	{
		addContent(e);
	}

	template <class T>
	void guiWidget(componentClass *component, uint32 &index)
	{
		entityManagerClass *ents = gui()->entities();

		for (uint32 scrollbars = 0; scrollbars < 2; scrollbars++)
		{
			if (scrollbars && !widgetScrollbars<T>::enable)
				continue;
			for (uint32 variation = 0; variation < widgetVariations<T>::count; variation++)
			{
				entityClass *panel = ents->createUnique();
				{
					GUI_GET_COMPONENT(panel, pan, panel);
					GUI_GET_COMPONENT(parent, p, panel);
					p.parent = 2;
					p.order = index++;
					GUI_GET_COMPONENT(layoutTable, lt, panel);
				}

				static const real small = 40;
				static const real large = 180;
				static const vec2 sizes[4] = {
					vec2(small, small),
					vec2(large, small),
					vec2(small, large),
					vec2(large, large)
				};

				for (uint32 i = 0; i < 4; i++)
				{
					entityClass *e = ents->createUnique();
					GUI_GET_COMPONENT(parent, parent, e);
					parent.parent = panel->name();
					parent.order = i;
					GUI_GET_COMPONENT(position, pos, e);
					pos.size.units[0] = unitEnum::Points;
					pos.size.units[1] = unitEnum::Points;
					pos.size.value = sizes[i];
					if (scrollbars)
					{
						GUI_GET_COMPONENT(scrollbars, sc, e);
					}
					e->add(component);
					widgetInit<T>(e, variation);
				}
			}
		}
	}

	void initialize() override
	{
		entityManagerClass *ents = gui()->entities();

		entityClass *panel = ents->create(2);
		{
			GUI_GET_COMPONENT(panel, pan, panel);
			GUI_GET_COMPONENT(position, pos, panel);
			pos.size.units[0] = unitEnum::ScreenWidth;
			pos.size.units[1] = unitEnum::ScreenHeight;
			pos.size.value = vec2(1, 1);
			GUI_GET_COMPONENT(layoutTable, lt, panel);
			lt.sections = 0;
			//lt.vertical = true;
			GUI_GET_COMPONENT(scrollbars, sc, panel);
			sc.overflow[0] = overflowModeEnum::Auto;
			sc.overflow[1] = overflowModeEnum::Always;
		}

		uint32 index = 0;

#define GCHL_GENERATE(T) guiWidget<CAGE_JOIN(T, Component)>(gui()->components().T, index);
		CAGE_EVAL_SMALL(CAGE_EXPAND_ARGS(GCHL_GENERATE, GCHL_GUI_WIDGET_COMPONENTS));
#undef GCHL_GENERATE
	}

};

MAIN(guiTestImpl, "widget sizes")
