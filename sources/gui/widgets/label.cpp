#include "../gui.h"

class guiTestImpl : public guiTestClass
{

	void initialize() override
	{
		guiBasicLayout();
	}

};

MAIN(guiTestImpl, "labels")
