#include <cage-core/logger.h>
#include <cage-engine/window.h>
#include <cage-engine/virtualReality.h>
#include <cage-engine/frameBuffer.h>
#include <cage-engine/texture.h>
#include <cage-engine/opengl.h>
#include <cage-engine/highPerformanceGpuHint.h>

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
		InputListener<InputClassEnum::WindowClose, InputWindow> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		windowCloseListener.attach(window->events);
		window->title("cage test virtual reality");
		window->windowedSize(Vec2i(800, 600));
		window->setWindowed();

		window->makeCurrent();
		Holder<VirtualReality> virtualreality = newVirtualReality({});

		InputListener<InputClassEnum::HeadsetConnected, InputHeadsetState> headsetConnectedListener;
		InputListener<InputClassEnum::HeadsetDisconnected, InputHeadsetState> headsetDisconnectedListener;
		InputListener<InputClassEnum::HeadsetPose, InputHeadsetPose> headsetPoseListener;
		InputListener<InputClassEnum::ControllerConnected, InputControllerState> controllerConnectedListener;
		InputListener<InputClassEnum::ControllerDisconnected, InputControllerState> controllerDisconnectedListener;
		InputListener<InputClassEnum::ControllerPose, InputControllerPose> controllerPoseListener;
		InputListener<InputClassEnum::ControllerPress, InputControllerKey> pressListener;
		InputListener<InputClassEnum::ControllerRelease, InputControllerKey> releaseListener;
		InputListener<InputClassEnum::ControllerAxis, InputControllerAxis> axisListener;
		headsetConnectedListener.attach(virtualreality->events);
		headsetDisconnectedListener.attach(virtualreality->events);
		headsetPoseListener.attach(virtualreality->events);
		controllerConnectedListener.attach(virtualreality->events);
		controllerDisconnectedListener.attach(virtualreality->events);
		controllerPoseListener.attach(virtualreality->events);
		pressListener.attach(virtualreality->events);
		releaseListener.attach(virtualreality->events);
		axisListener.attach(virtualreality->events);
		headsetConnectedListener.bind<headsetConnected>();
		headsetDisconnectedListener.bind<headsetDiconnected>();
		headsetPoseListener.bind<headsetPose>();
		controllerConnectedListener.bind<controllerConnected>();
		controllerDisconnectedListener.bind<controllerDiconnected>();
		controllerPoseListener.bind<controllerPose>();
		pressListener.bind<press>();
		releaseListener.bind<release>();
		axisListener.bind<axis>();

		Holder<FrameBuffer> fb = newFrameBufferDraw();

		while (!closing)
		{
			window->processEvents();
			virtualreality->processEvents();

			Holder<VirtualRealityGraphicsFrame> frame = virtualreality->graphicsFrame();
			frame->updateProjections();
			fb->bind();

			frame->renderBegin();
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
