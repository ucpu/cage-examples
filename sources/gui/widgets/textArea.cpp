#include "../gui.h"
#include <cage-core/memoryBuffer.h>

class GuiTestImpl : public GuiTestClass
{
public:
	static MemoryBuffer makeBufferWithText()
	{
		static constexpr const char str[] = R"foo(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque ut convallis metus. Donec vestibulum nisl vel ex faucibus, eu maximus metus maximus. Nam elementum tellus sed cursus rhoncus. Donec porttitor varius dignissim. Mauris aliquam tortor quis dolor semper posuere. Morbi risus felis, fermentum et lectus ut, ultricies tempus nisl. Integer vitae mi auctor, vestibulum lorem ac, rhoncus urna.
In aliquam tellus diam, non bibendum lectus commodo eget. Donec imperdiet ante metus, et pretium lorem commodo ut. Donec ultrices, nisi sed tempus condimentum, sem lorem euismod nisl, aliquam tincidunt lorem dui eu erat. Suspendisse pharetra tincidunt dolor. Sed nibh odio, ultricies nec ligula sed, venenatis blandit odio. Proin porttitor laoreet quam, id ornare sapien sollicitudin eu. Fusce egestas enim lacus, in venenatis ante lobortis eget.
Praesent quis tincidunt odio. Nunc a nisl scelerisque, rhoncus ligula non, rutrum metus. Nullam non quam magna. Sed blandit, diam ac finibus rutrum, nibh dolor vulputate purus, ut commodo nisi massa bibendum enim. Proin lacinia ut orci ut tempor. Nam eget scelerisque risus. Aliquam vulputate suscipit ipsum ut condimentum. Nulla consectetur commodo rhoncus. Quisque sed rutrum arcu. Nunc varius leo vitae mauris tempor, non ornare tortor pretium. Nam fringilla tellus neque, vel faucibus diam lobortis ac. Etiam a viverra arcu. Quisque convallis magna sed ipsum auctor suscipit. Phasellus rutrum at urna non malesuada.

Nulla et ipsum ligula. Nunc tristique ut felis in tristique. Suspendisse sit amet suscipit eros. Proin fringilla lorem eget neque faucibus, consectetur aliquam lacus varius. Duis porta, quam non laoreet aliquam, dolor tortor ullamcorper massa, ultricies eleifend dolor ex id purus. Mauris bibendum est eu purus ullamcorper efficitur. Maecenas sollicitudin neque nec urna placerat viverra. Cras et malesuada lectus. Nulla ac dictum lorem, quis ornare nulla.
)foo";

		MemoryBuffer buf;
		buf.resize(sizeof(str));
		detail::memcpy(buf.data(), str, sizeof(str));
		return buf;
	}

	MemoryBuffer buffer = makeBufferWithText();

	void initialize() override
	{
		EntityManager *ents = engineGuiEntities();

		guiBasicLayout();
		{
			Entity *e = ents->get(3);
			e->value<GuiLayoutTableComponent>();
		}

		uint32 index = 1;

		{ // default
			guiLabel(3, index, "default");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiTextAreaComponent>().buffer = &buffer;
			e->value<GuiTooltipComponent>().tooltip = detail::guiTooltipText<"tooltip: in case you need more text for this text">();
		}

		{ // disabled
			guiLabel(3, index, "disabled");
			Entity *e = ents->createUnique();
			GuiParentComponent &p = e->value<GuiParentComponent>();
			p.parent = 3;
			p.order = index++;
			e->value<GuiTextAreaComponent>().buffer = &buffer;
			e->value<GuiWidgetStateComponent>().disabled = true;
		}
	}
};

MAIN(GuiTestImpl, "text area")
