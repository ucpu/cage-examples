#include <cage-core/logger.h>
#include <cage-engine/frameBuffer.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/opengl.h>
#include <cage-engine/texture.h>
#include <cage-engine/virtualReality.h>
#include <cage-engine/window.h>

using namespace cage;

bool closing = false;

void windowClose(InputWindow)
{
	closing = true;
}

void headsetConnected(InputHeadsetState in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset connected");
}
void headsetDiconnected(InputHeadsetState in)
{
	CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset disconnected");
}
void headsetPose(InputHeadsetPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "headset", Stringizer() + "headset pose: " + in.pose);
}
void controllerConnected(InputControllerState in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller connected");
}
void controllerDiconnected(InputControllerState in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller disconnected");
}
void controllerPose(InputControllerPose in)
{
	//CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "controller grip pose: " + in.pose);
}
void press(InputControllerKey in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "press: " + in.key);
}
void release(InputControllerKey in)
{
	CAGE_LOG(SeverityEnum::Info, "controller", Stringizer() + "release: " + in.key);
}
void axis(InputControllerAxis in)
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
		const auto closeListener = window->events.listen(inputListener<InputClassEnum::WindowClose, InputWindow>(&windowClose));
		window->title("cage test virtual reality");
		window->windowedSize(Vec2i(800, 600));
		window->setWindowed();
		window->makeCurrent();

		Holder<VirtualReality> virtualreality = newVirtualReality({});
		const auto headsetConnectedListener = virtualreality->events.listen(inputListener<InputClassEnum::HeadsetConnected, InputHeadsetState>([](auto in) { return headsetConnected(in); }));
		const auto headsetDisconnectedListener = virtualreality->events.listen(inputListener<InputClassEnum::HeadsetDisconnected, InputHeadsetState>([](auto in) { return headsetDiconnected(in); }));
		const auto headsetPoseListener = virtualreality->events.listen(inputListener<InputClassEnum::HeadsetPose, InputHeadsetPose>([](auto in) { return headsetPose(in); }));
		const auto controllerConnectedListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerConnected, InputControllerState>([](auto in) { return controllerConnected(in); }));
		const auto controllerDisconnectedListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerDisconnected, InputControllerState>([](auto in) { return controllerDiconnected(in); }));
		const auto controllerPoseListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerPose, InputControllerPose>([](auto in) { return controllerPose(in); }));
		const auto pressListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerPress, InputControllerKey>([](auto in) { return press(in); }));
		const auto releaseListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerRelease, InputControllerKey>([](auto in) { return release(in); }));
		const auto axisListener = virtualreality->events.listen(inputListener<InputClassEnum::ControllerAxis, InputControllerAxis>([](auto in) { return axis(in); }));

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
