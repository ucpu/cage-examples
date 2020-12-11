#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>

#include <cage-engine/sound.h>
#include <cage-engine/speakerList.h>

using namespace cage;

void testDevice(const string &deviceId, uint32 sampleRate)
{
	Holder<SoundContext> sndContext = newSoundContext(SoundContextCreateConfig(), "testAudio:Context");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "backend: '" + sndContext->getBackendName() + "'");

	Holder<MixingBus> sndBus = newMixingBus();
	Holder<SoundSource> sndSource = newSoundSource();
	sndSource->addOutput(+sndBus);
	sndSource->setDataTone();
	SpeakerCreateConfig cnf;
	cnf.deviceId = deviceId;
	cnf.sampleRate = sampleRate;
	Holder<Speaker> sndSpeaker = newSpeakerOutput(+sndContext, cnf, "testAudio:Speaker");
	sndSpeaker->setInput(+sndBus);

	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "stream name: '" + sndSpeaker->getStreamName() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "device id: '" + sndSpeaker->getDeviceId() + "'");
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "channels: " + sndSpeaker->getChannels());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "sample rate: " + sndSpeaker->getSamplerate());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "latency: " + sndSpeaker->getLatency());

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
		for (const SpeakerDevice &d : list->devices())
		{
			CAGE_LOG(SeverityEnum::Info, "listing", "-------------------------------------");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "id: '" + d.id + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "name: '" + d.name + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "group: '" + d.group + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "vendor: '" + d.vendor + "'");
			if (d.channels)
			{
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "channels: " + d.channels);
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "sample rate min: " + d.minSamplerate + ", max: " + d.maxSamplerate + ", default: " + d.defaultSamplerate);
				testDevice(d.id, 44100);
				testDevice(d.id, 48000);
			}
		}

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
