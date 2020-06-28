#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>

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
		uint32 defaultDevice = list->defaultDevice();
		for (const SpeakerDevice *d : list->devices())
		{
			CAGE_LOG(SeverityEnum::Info, "listing", stringizer() + "device" + (d->raw() ? ", raw" : "") + (defaultDevice-- == 0 ? ", default" : ""));
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "name: '" + d->name() + "'");
			{
				uint32 currentLayout = d->currentLayout();
				for (const SpeakerLayout &l : d->layouts())
					CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "layout name: '" + l.name + "', channels: " + l.channels + (currentLayout-- == 0 ? ", current" : ""));
			}
			{
				for (const SpeakerSamplerate &s : d->samplerates())
					CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "samplerate min: " + s.minimum + ", max: " + s.maximum);
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "samplerate current: " + d->currentSamplerate());
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
		detail::logCurrentCaughtException();
		return 1;
	}
}
