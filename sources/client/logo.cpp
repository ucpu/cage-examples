#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/assetStructs.h>
#include <cage-core/assetManager.h>
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
		holder<logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		// window
		holder<windowHandle> window = newWindow();
		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		window->events.windowClose.attach(windowCloseListener);
		window->title("cage test logo");
		detail::initializeOpengl();

		// sound
		holder<soundContext> sl = newSoundContext(soundContextCreateConfig(), "cage");

		// assets
		holder<assetManager> assets = newAssetManager(assetManagerCreateConfig());
		assets->defineScheme<void>(assetSchemeIndexPack, genAssetSchemePack(0));
		assets->defineScheme<shaderProgram>(assetSchemeIndexShaderProgram, genAssetSchemeShaderProgram(0, window.get()));
		assets->defineScheme<renderTexture>(assetSchemeIndexRenderTexture, genAssetSchemeRenderTexture(0, window.get()));
		assets->defineScheme<renderMesh>(assetSchemeIndexMesh, genAssetSchemeRenderMesh(0, window.get()));
		assets->defineScheme<fontFace>(assetSchemeIndexFontFace, genAssetSchemeFontFace(0, window.get()));
		assets->defineScheme<soundSource>(assetSchemeIndexSoundSource, genAssetSchemeSoundSource(0, sl.get()));

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
		renderMesh *mesh = assets->get<assetSchemeIndexMesh, renderMesh>(hashString("cage/mesh/square.obj"));
		renderTexture *texture = assets->get<assetSchemeIndexRenderTexture, renderTexture>(hashString("cage-tests/logo/logo.png"));
		shaderProgram *shader = assets->get<assetSchemeIndexShaderProgram, shaderProgram>(hashString("cage/shader/engine/blit.glsl"));
		soundSource *source = assets->get<assetSchemeIndexSoundSource, soundSource>(hashString("cage-tests/logo/logo.ogg"));

		{
			// initialize graphics
			mesh->bind();
			glActiveTexture(GL_TEXTURE0 + CAGE_SHADER_TEXTURE_COLOR);
			texture->bind();
			shader->bind();

			// initialize sounds
			holder<speakerOutput> speaker = newSpeakerOutput(sl.get(), speakerOutputCreateConfig(), "cage");
			holder<mixingBus> bus = newMixingBus(sl.get());
			speaker->setInput(bus.get());
			source->addOutput(bus.get());

			// show the window
			ivec2 res(600, 600);
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
