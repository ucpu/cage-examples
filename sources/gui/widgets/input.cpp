#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		EntityManager *ents = engineGui()->entities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(LayoutTable, layout, e);
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
		}

		{ // integer
			guiLabel(3, index, "integer");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
			ib.type = InputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
		}

		{ // real
			guiLabel(3, index, "real");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
			ib.type = InputTypeEnum::Real;
			ib.min.f = -1;
			ib.max.f = 1;
			ib.step.f = 0.1;
		}

		{ // password
			guiLabel(3, index, "password");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
			ib.type = InputTypeEnum::Password;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "password";
		}

		{ // email
			guiLabel(3, index, "email");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
			ib.type = InputTypeEnum::Email;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "@";
		}

		{ // url
			guiLabel(3, index, "url");
			Entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(Parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(Input, ib, e);
			ib.type = InputTypeEnum::Url;
			CAGE_COMPONENT_GUI(Text, t, e);
			t.value = "http://";
		}
	}

};

MAIN(guiTestImpl, "input fields")
