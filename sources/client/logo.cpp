#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/assets.h>
#include <cage-core/timer.h>
#include <cage-core/hashString.h>
#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/graphics.h>
#include <cage-client/sound.h>
#include <cage-client/opengl.h>
#include <cage-client/highPerformanceGpuHint.h>
#include <cage-client/graphics/shaderConventions.h>

using namespace cage;

bool closing = false;
const uint32 assetsName = hashString("cage-tests/logo/logo.pack");

bool windowClose()
{
	closing = true;
	return false;
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		// window
		holder<windowClass> window = newWindow();
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		window->events.windowClose.attach(windowCloseListener);
		window->title("cage test logo");
		detail::initializeOpengl();

		// sound
		holder<soundContextClass> sl = newSoundContext(soundContextCreateConfig(), "cage");

		// assets
		holder<assetManagerClass> assets = newAssetManager(assetManagerCreateConfig());
		assets->defineScheme<void>(assetSchemeIndexPack, genAssetSchemePack(0));
		assets->defineScheme<shaderClass>(assetSchemeIndexShader, genAssetSchemeShader(0, window.get()));
		assets->defineScheme<textureClass>(assetSchemeIndexTexture, genAssetSchemeTexture(0, window.get()));
		assets->defineScheme<meshClass>(assetSchemeIndexMesh, genAssetSchemeMesh(0, window.get()));
		assets->defineScheme<fontClass>(assetSchemeIndexFont, genAssetSchemeFont(0, window.get()));
		assets->defineScheme<sourceClass>(assetSchemeIndexSound, genAssetSchemeSound(0, sl.get()));

		// load assets
		assets->add(assetsName);
		while (true)
		{
			if (assets->ready(assetsName))
				break;
			assets->processControlThread();
			assets->processCustomThread(0);
		}

		// fetch assets
		meshClass *mesh = assets->get<assetSchemeIndexMesh, meshClass>(hashString("cage/mesh/square.obj"));
		textureClass *texture = assets->get<assetSchemeIndexTexture, textureClass>(hashString("cage-tests/logo/logo.png"));
		shaderClass *shader = assets->get<assetSchemeIndexShader, shaderClass>(hashString("cage/shader/engine/blit.glsl"));
		sourceClass *source = assets->get<assetSchemeIndexSound, sourceClass>(hashString("cage-tests/logo/logo.ogg"));

		{
			// initialize graphics
			mesh->bind();
			glActiveTexture(GL_TEXTURE0 + CAGE_SHADER_TEXTURE_COLOR);
			texture->bind();
			shader->bind();

			// initialize sounds
			holder<speakerClass> speaker = newSpeaker(sl.get(), speakerCreateConfig(), "cage");
			holder<busClass> bus = newBus(sl.get());
			speaker->setInput(bus.get());
			source->addOutput(bus.get());

			// show the window
			pointStruct res(600, 600);
			window->windowedSize(res);
			window->setWindowed();

			// loop
			while (!closing)
			{
				res = window->resolution();
				glViewport(0, 0, res.x, res.y);
				mesh->dispatch();
				speaker->update(getApplicationTime());
				threadSleep(10000);
				window->swapBuffers();
				window->processEvents();
			}
		}

		// unload assets
		assets->remove(assetsName);
		while (assets->countTotal())
		{
			assets->processControlThread();
			assets->processCustomThread(0);
		}

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
