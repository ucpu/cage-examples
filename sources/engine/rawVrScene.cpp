#include <cage-core/assetContext.h>
#include <cage-core/assetManager.h>
#include <cage-core/concurrent.h>
#include <cage-core/hashString.h>
#include <cage-core/logger.h>
#include <cage-engine/frameBuffer.h>
#include <cage-engine/highPerformanceGpuHint.h>
#include <cage-engine/model.h>
#include <cage-engine/opengl.h>
#include <cage-engine/renderObject.h>
#include <cage-engine/shaderProgram.h>
#include <cage-engine/texture.h>
#include <cage-engine/virtualReality.h>
#include <cage-engine/window.h>

using namespace cage;

bool closing = false;
constexpr uint32 assetsName1 = HashString("scenes/mcguire/crytek/sponza-preload.object");
constexpr uint32 assetsName2 = HashString("cage-tests/vr/vr.pack");

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

		// assets
		Holder<AssetManager> assets = newAssetManager(AssetManagerCreateConfig());
		assets->defineScheme<AssetSchemeIndexPack, AssetPack>(genAssetSchemePack());
		assets->defineScheme<AssetSchemeIndexShaderProgram, MultiShaderProgram>(genAssetSchemeShaderProgram(0));
		assets->defineScheme<AssetSchemeIndexTexture, Texture>(genAssetSchemeTexture(0));
		assets->defineScheme<AssetSchemeIndexModel, Model>(genAssetSchemeModel(0));
		assets->defineScheme<AssetSchemeIndexRenderObject, RenderObject>(genAssetSchemeRenderObject());

		// load assets
		assets->load(assetsName1);
		assets->load(assetsName2);
		while (assets->processing())
		{
			assets->processCustomThread(0);
			threadSleep(1000);
		}

		{
			// fetch assets
			Holder<RenderObject> sponza = assets->get<AssetSchemeIndexRenderObject, RenderObject>(assetsName1);
			Holder<ShaderProgram> shader = assets->get<AssetSchemeIndexShaderProgram, MultiShaderProgram>(HashString("cage-tests/vr/raw.glsl"))->get(0);
			Holder<Model> gripModel = assets->get<AssetSchemeIndexModel, Model>(HashString("cage-tests/vr/grip.obj"));
			Holder<Model> aimModel = assets->get<AssetSchemeIndexModel, Model>(HashString("cage-tests/vr/aim.obj"));
			shader->bind();

			// show window
			window->windowedSize(Vec2i(800, 600));
			window->setWindowed();
			Holder<VirtualReality> virtualreality = newVirtualReality();
			Holder<FrameBuffer> fb = newFrameBufferDraw();
			Holder<Texture> depthTexture = newTexture();
			depthTexture->setDebugName("depth texture");

			while (!closing)
			{
				window->processEvents();

				// clear the window
				const Vec2i res = window->resolution();
				glViewport(0, 0, res[0], res[1]);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClear(GL_COLOR_BUFFER_BIT);
				window->swapBuffers();

				// render to headset
				virtualreality->processEvents();
				Holder<VirtualRealityGraphicsFrame> frame = virtualreality->graphicsFrame();
				frame->updateProjections();
				fb->bind();

				if (depthTexture->internalFormat() == 0)
				{
					depthTexture->bind(0);
					depthTexture->initialize(frame->cameras[0].resolution, 1, GL_DEPTH_COMPONENT32);
					fb->depthTexture(+depthTexture);
				}
				glEnable(GL_DEPTH_TEST);

				frame->renderBegin();
				frame->acquireTextures();
				for (const auto &view : frame->cameras)
				{
					if (!view.colorTexture)
						continue;

					fb->colorTexture(0, view.colorTexture);
					fb->checkStatus();

					glViewport(0, 0, view.resolution[0], view.resolution[1]);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					Mat4 viewM = inverse(Mat4(view.transform));
					Mat4 projM = view.projection;
					shader->uniform(0, projM * viewM);

					for (uint32 name : sponza->models(0))
					{
						Holder<Model> model = assets->get<AssetSchemeIndexModel, Model>(name);
						if (!model->textureNames[0])
							continue;
						model->bind();
						assets->get<AssetSchemeIndexTexture, Texture>(model->textureNames[0])->bind(0);
						model->dispatch();
					}

					for (const VirtualRealityController *it : { &virtualreality->leftController(), &virtualreality->rightController() })
					{
						const Mat4 sc = Mat4::scale(it->tracking() ? 1 : 0.8);

						shader->uniform(0, projM * viewM * Mat4(it->gripPose()) * sc);
						gripModel->bind();
						assets->get<AssetSchemeIndexTexture, Texture>(gripModel->textureNames[0])->bind(0);
						gripModel->dispatch();

						shader->uniform(0, projM * viewM * Mat4(it->aimPose()) * sc);
						aimModel->bind();
						assets->get<AssetSchemeIndexTexture, Texture>(aimModel->textureNames[0])->bind(0);
						aimModel->dispatch();
					}
				}
				frame->renderCommit();
			}
		}

		// unload assets
		assets->unload(assetsName1);
		assets->unload(assetsName2);
		assets->unloadCustomThread(0);

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
