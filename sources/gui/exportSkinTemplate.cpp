#include "gui.h"

#include <cage-core/png.h>

class guiTestImpl : public guiTestClass
{
public:

	void initialize() override
	{
		detail::guiSkinTemplateExport(gui()->skin(0), 2048)->encodeFile("guiSkinTemplate.png");
		engineStop();
	}

};

MAIN(guiTestImpl, "export skin template")
