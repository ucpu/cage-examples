#include "../gui.h"

#include <cage-core/memoryBuffer.h>
#include <cage-core/serialization.h>

class guiTestImpl : public guiTestClass
{

	memoryBuffer buff[4];

	void initialize() override
	{
		guiBasicLayout();

		entityManagerClass *ents = gui()->entities();
		uint32 index = 1;

		{ // limited
			guiLabel(3, index, "limited");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(textArea, ta, e);
			ta.buffer = &buff[0];
			ta.maxLength = 20;
		}

		{ // prefilled
			guiLabel(3, index, "prefilled");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(textArea, ta, e);
			ta.buffer = &buff[1];
		}

		{ // shared 1
			guiLabel(3, index, "shared 1");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(textArea, ta, e);
			ta.buffer = &buff[2];
		}

		{ // shared 2
			guiLabel(3, index, "shared 2");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(textArea, ta, e);
			ta.buffer = &buff[2];
		}

		{ // read only
			guiLabel(3, index, "read only");
			entityClass *e = ents->createUnique();
			GUI_GET_COMPONENT(parent, p, e);
			p.parent = 3;
			p.order = index++;
			GUI_GET_COMPONENT(textArea, ta, e);
			ta.buffer = &buff[3];
			ta.style |= inputStyleFlags::ReadOnly;
		}

		// prefill the buffers
		for (uint32 i = 1; i < 4; i++)
		{
			static const string words[] = {
				"lorem",
				"ipus",
				"dolor",
				"sit",
				"amet",
				"haha",
				"paramount",
				"grenade",
				"over",
				"is",
				"a",
				"the",
				"lol",
				"omg",
				"this",
				"how",
				"you",
				"grass",
				"about",
				"love",
				"master",
				"button",
				"read",
				"want",
				"dream",
				"little",
				"longer",
				"let",
				"close",
				"door",
			};
			static const uint32 count = sizeof(words) / sizeof(words[0]);
			serializer s(buff[i]);
			uint32 cnt = randomRange(5, 100);
			for (uint32 i = 0; i < cnt; i++)
			{
				if (buff[i].size() > 0)
					s.write(" ", 1);
				uint32 j = randomRange(0u, count - 1);
				s.write(words[j].c_str(), words[j].length());
			}
		}
	}

};

MAIN(guiTestImpl, "text areas")
