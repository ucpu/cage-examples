#include <cage-core/core.h>
#include <cage-core/math.h>
#include <cage-core/log.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>
#include <cage-engine/core.h>
#include <cage-engine/sound.h>
#include <cage-engine/speakerList.h>

using namespace cage;

void testDevice(const string &deviceId, uint32 sampleRate, bool raw)
{
	holder<soundContext> sndContext = newSoundContext(soundContextCreateConfig(), "testAudio:Context");
	holder<mixingBus> sndBus = newMixingBus(sndContext.get());
	holder<soundSource> sndSource = newSoundSource(sndContext.get());
	sndSource->addOutput(sndBus.get());
	sndSource->setDataTone();
	speakerOutputCreateConfig cnf;
	cnf.deviceId = deviceId;
	cnf.sampleRate = sampleRate;
	cnf.deviceRaw = raw;
	holder<speakerOutput> sndSpeaker = newSpeakerOutput(sndContext.get(), cnf, "testAudio:Speaker");
	sndSpeaker->setInput(sndBus.get());

	CAGE_LOG(severityEnum::Info, "speaker", string() + "stream: '" + sndSpeaker->getStreamName() + "'");
	CAGE_LOG(severityEnum::Info, "speaker", string() + "id: '" + sndSpeaker->getDeviceId() + "'");
	CAGE_LOG(severityEnum::Info, "speaker", string() + "name: '" + sndSpeaker->getDeviceName() + "'");
	CAGE_LOG(severityEnum::Info, "speaker", string() + "raw: " + sndSpeaker->getDeviceRaw());
	CAGE_LOG(severityEnum::Info, "speaker", string() + "layout: '" + sndSpeaker->getLayoutName() + "'");
	CAGE_LOG(severityEnum::Info, "speaker", string() + "channels: " + sndSpeaker->getChannelsCount());
	CAGE_LOG(severityEnum::Info, "speaker", string() + "sample rate: " + sndSpeaker->getOutputSampleRate());

	CAGE_LOG(severityEnum::Info, "speaker", "play start");
	holder<timer> tmr = newTimer();
	while (true)
	{
		uint64 t = tmr->microsSinceStart();
		if (t > 3 * 1000 * 1000)
			break;
		sndSpeaker->update(t);
		threadSleep(10000);
	}
	CAGE_LOG(severityEnum::Info, "speaker", "play stop");
}

int main(int argc, char *args[])
{
	try
	{
		// log to console
		holder<logger> log1 = newLogger();
		log1->format.bind <logFormatConsole>();
		log1->output.bind <logOutputStdOut>();

		holder<speakerList> list = newSpeakerList();
		uint32 dc = list->devicesCount(), dd = list->defaultDevice();
		for (uint32 di = 0; di < dc; di++)
		{
			const speakerDevice *d = list->device(di);
			CAGE_LOG(severityEnum::Info, "listing", string() + "device" + (d->raw() ? ", raw" : "") + (dd == di ? ", default" : ""));
			CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "id: '" + d->id() + "'");
			CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "name: '" + d->name() + "'");
			{
				uint32 lc = d->layoutsCount(), ld = d->currentLayout();
				for (uint32 li = 0; li < lc; li++)
				{
					const speakerLayout &l = d->layout(li);
					CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "layout name: '" + l.name + "', channels: " + l.channels + (li == ld ? ", current" : ""));
				}
			}
			{
				uint32 sc = d->sampleratesCount(), sd = d->currentSamplerate();
				for (uint32 si = 0; si < sc; si++)
				{
					const speakerSamplerate &s = d->samplerate(si);
					CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "samplerate min: " + s.minimum + ", max: " + s.maximum);
				}
				CAGE_LOG_CONTINUE(severityEnum::Info, "listing", string() + "samplerate current: " + sd);
			}
			if (d->raw())
				continue;
			testDevice(d->id(), 32000, false);
			testDevice(d->id(), 44100, false);
			testDevice(d->id(), 48000, false);
		}

		return 0;
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "test", "caught exception");
		return 1;
	}
}
