#include <cage-core/logger.h>
#include <cage-engine/frameBuffer.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/opengl.h>
#include <cage-engine/texture.h>
#include <cage-engine/virtualReality.h>
#include <cage-engine/window.h>

using namespace cage;

bool closing = false;

void headsetConnected(input::HeadsetConnected in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset connected");
}
void headsetDiconnected(input::HeadsetDisconnected in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset disconnected");
}
void headsetPose(input::HeadsetPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset pose: " + in.pose);
}
void controllerConnected(input::ControllerConnected in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller connected");
}
void controllerDiconnected(input::ControllerDisconnected in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller disconnected");
}
void controllerPose(input::ControllerPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller grip pose: " + in.pose);
}
void press(input::ControllerPress in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "press: " + in.key);
}
void release(input::ControllerRelease in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "release: " + in.key);
}
void axis(input::ControllerAxis in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "axis: " + in.axis + ", value: " + in.value);
}

int main(int argc, char *args[])
{
	try
	{
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		Holder<Window> window = newWindow(WindowCreateConfig{ .vsync = 0 });
		const auto closeListener = window->events.listen(inputFilter([](input::WindowClose) { closing = true; }));
		window->title("cage test virtual reality");
		window->windowedSize(Vec2i(800, 600));
		window->setWindowed();
		window->makeCurrent();

		Holder<VirtualReality> virtualreality = newVirtualReality();
		const auto headsetConnectedListener = virtualreality->events.listen(inputFilter(&headsetConnected));
		const auto headsetDisconnectedListener = virtualreality->events.listen(inputFilter(&headsetDiconnected));
		const auto headsetPoseListener = virtualreality->events.listen(inputFilter(&headsetPose));
		const auto controllerConnectedListener = virtualreality->events.listen(inputFilter(&controllerConnected));
		const auto controllerDisconnectedListener = virtualreality->events.listen(inputFilter(&controllerDiconnected));
		const auto controllerPoseListener = virtualreality->events.listen(inputFilter(&controllerPose));
		const auto pressListener = virtualreality->events.listen(inputFilter(press));
		const auto releaseListener = virtualreality->events.listen(inputFilter(release));
		const auto axisListener = virtualreality->events.listen(inputFilter(axis));

		Holder<FrameBuffer> fb = newFrameBufferDraw();

		while (!closing)
		{
			window->processEvents();
			virtualreality->processEvents();

			Holder<VirtualRealityGraphicsFrame> frame = virtualreality->graphicsFrame();
			frame->updateProjections();
			fb->bind();

			frame->renderBegin();
			frame->acquireTextures();
			uint32 index = 0;
			for (const auto &view : frame->cameras)
			{
				if (!view.colorTexture)
					continue;

				fb->colorTexture(0, view.colorTexture);
				fb->checkStatus();

				glViewport(0, 0, view.resolution[0], view.resolution[1]);
				glClearColor((index + 1) % 2, index % 2, 0, 0);
				glClear(GL_COLOR_BUFFER_BIT);
				index++;
			}
			frame->renderCommit();

			const Vec2i res = window->resolution();
			glViewport(0, 0, res[0], res[1]);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0, 0, 1, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			window->swapBuffers();
		}

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
