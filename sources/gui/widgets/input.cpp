#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
		}

		{ // integer
			guiLabel(3, index, "integer");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
			ib.type = inputTypeEnum::Integer;
			ib.min.i = -13;
			ib.max.i = 42;
			ib.step.i = 5;
		}

		{ // real
			guiLabel(3, index, "real");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
			ib.type = inputTypeEnum::Real;
			ib.min.f = -1;
			ib.max.f = 1;
			ib.step.f = 0.1;
		}

		{ // password
			guiLabel(3, index, "password");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
			ib.type = inputTypeEnum::Password;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "password";
		}

		{ // email
			guiLabel(3, index, "email");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
			ib.type = inputTypeEnum::Email;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "@";
		}

		{ // url
			guiLabel(3, index, "url");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(input, ib, e);
			ib.type = inputTypeEnum::Url;
			GUI_GET_COMPONENT(text, t, e);
			t.value = "http://";
		}
	}

};

MAIN(guiTestImpl, "input fields")
