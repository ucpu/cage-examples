#include <cage-core/image.h>
#include <cage-engine/guiSkins.h>

#include "gui.h"

class GuiTestImpl : public GuiTestClass
{
public:
	void initialize() override
	{
		detail::guiSkinTemplateExport(engineGuiManager()->skin(0), 2048)->exportFile("guiSkinTemplate.png");
		engineStop();
	}
};

MAIN(GuiTestImpl, "export skin template")
