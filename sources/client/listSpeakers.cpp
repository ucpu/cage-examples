#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>
#include <cage-engine/core.h>
#include <cage-engine/sound.h>
#include <cage-engine/speakerList.h>

using namespace cage;

void testDevice(const string &deviceId, uint32 sampleRate, bool raw)
{
	Holder<SoundContext> sndContext = newSoundContext(SoundContextCreateConfig(), "testAudio:Context");
	Holder<MixingBus> sndBus = newMixingBus(sndContext.get());
	Holder<SoundSource> sndSource = newSoundSource(sndContext.get());
	sndSource->addOutput(sndBus.get());
	sndSource->setDataTone();
	SpeakerCreateConfig cnf;
	cnf.deviceId = deviceId;
	cnf.sampleRate = sampleRate;
	cnf.deviceRaw = raw;
	Holder<Speaker> sndSpeaker = newSpeakerOutput(sndContext.get(), cnf, "testAudio:Speaker");
	sndSpeaker->setInput(sndBus.get());

	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "stream: '" + sndSpeaker->getStreamName() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "id: '" + sndSpeaker->getDeviceId() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "name: '" + sndSpeaker->getDeviceName() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "raw: " + sndSpeaker->getDeviceRaw());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "layout: '" + sndSpeaker->getLayoutName() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "channels: " + sndSpeaker->getChannelsCount());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "sample rate: " + sndSpeaker->getOutputSampleRate());

	CAGE_LOG(SeverityEnum::Info, "speaker", "play start");
	Holder<Timer> tmr = newTimer();
	while (true)
	{
		uint64 t = tmr->microsSinceStart();
		if (t > 3 * 1000 * 1000)
			break;
		sndSpeaker->update(t);
		threadSleep(10000);
	}
	CAGE_LOG(SeverityEnum::Info, "speaker", "play stop");
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		Holder<Logger> log1 = newLogger();
		log1->format.bind<logFormatConsole>();
		log1->output.bind<logOutputStdOut>();

		Holder<SpeakerList> list = newSpeakerList();
		uint32 dc = list->devicesCount();
		uint32 dd = list->defaultDevice();
		for (uint32 di = 0; di < dc; di++)
		{
			const SpeakerDevice *d = list->device(di);
			CAGE_LOG(SeverityEnum::Info, "listing", stringizer() + "device" + (d->raw() ? ", raw" : "") + (dd == di ? ", default" : ""));
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "name: '" + d->name() + "'");
			{
				uint32 lc = d->layoutsCount(), ld = d->currentLayout();
				for (uint32 li = 0; li < lc; li++)
				{
					const SpeakerLayout &l = d->layout(li);
					CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "layout name: '" + l.name + "', channels: " + l.channels + (li == ld ? ", current" : ""));
				}
			}
			{
				uint32 sc = d->sampleratesCount(), sd = d->currentSamplerate();
				for (uint32 si = 0; si < sc; si++)
				{
					const SpeakerSamplerate &s = d->samplerate(si);
					CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "samplerate min: " + s.minimum + ", max: " + s.maximum);
				}
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "samplerate current: " + sd);
			}
			if (d->raw())
				continue;
			testDevice(d->id(), 44100, false);
			testDevice(d->id(), 48000, false);
		}

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(SeverityEnum::Error, "test", "caught exception");
		return 1;
	}
}
