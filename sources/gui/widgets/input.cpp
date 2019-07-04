#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		entityManager *ents = gui()->entities();

		guiBasicLayout();
		{
			entity *e = ents->get(3);
			CAGE_COMPONENT_GUI(layoutTable, layout, e);
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
		}

		{ // integer
			guiLabel(3, index, "integer");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
			ib.type = inputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
		}

		{ // real
			guiLabel(3, index, "real");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
			ib.type = inputTypeEnum::Real;
			ib.min.f = -1;
			ib.max.f = 1;
			ib.step.f = 0.1;
		}

		{ // password
			guiLabel(3, index, "password");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
			ib.type = inputTypeEnum::Password;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "password";
		}

		{ // email
			guiLabel(3, index, "email");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
			ib.type = inputTypeEnum::Email;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "@";
		}

		{ // url
			guiLabel(3, index, "url");
			entity *e = ents->createUnique();
			CAGE_COMPONENT_GUI(parent, p, e);
			p.parent = 3;
			p.order = index++;
			CAGE_COMPONENT_GUI(input, ib, e);
			ib.type = inputTypeEnum::Url;
			CAGE_COMPONENT_GUI(text, t, e);
			t.value = "http://";
		}
	}

};

MAIN(guiTestImpl, "input fields")
