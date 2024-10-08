#include <cmath>

#include <cage-core/concurrent.h>
#include <cage-core/logger.h>
#include <cage-core/timer.h>
#include <cage-engine/speaker.h>
#include <cage-engine/speakerList.h>

using namespace cage;

void synthesizeTone(const SoundCallbackData &data)
{
	CAGE_ASSERT(data.buffer.size() == data.frames * data.channels);
	constexpr double twoPi = 3.14159265358979323846264338327950288419716939937510 * 2;
	constexpr double pitch = 440;
	const double step = twoPi * pitch / data.sampleRate;
	const double offset = data.time * twoPi * pitch / 1000000;
	for (uint32 sampleIndex = 0; sampleIndex < data.frames; sampleIndex++)
	{
		double angle = sampleIndex * step + offset;
		Real sample = std::sin(angle);
		for (uint32 ch = 0; ch < data.channels; ch++)
			data.buffer[sampleIndex * data.channels + ch] = sample.value;
	}
}

void testDevice(const String &deviceId, uint32 sampleRate)
{
	CAGE_LOG(SeverityEnum::Info, "listing", "-----------------");

	SpeakerCreateConfig cfg;
	cfg.deviceId = deviceId;
	cfg.sampleRate = sampleRate;
	cfg.callback = Delegate<void(const SoundCallbackData &)>().bind<&synthesizeTone>();
	Holder<Speaker> speaker = newSpeaker(cfg);
	CAGE_LOG(SeverityEnum::Info, "speaker", Stringizer() + "channels: " + speaker->channels());
	CAGE_LOG(SeverityEnum::Info, "speaker", Stringizer() + "sample rate: " + speaker->sampleRate());
	CAGE_LOG(SeverityEnum::Info, "speaker", Stringizer() + "latency: " + speaker->latency());
	speaker->start();

	CAGE_LOG(SeverityEnum::Info, "speaker", "play start");
	Holder<Timer> tmr = newTimer();
	while (true)
	{
		uint64 t = tmr->duration();
		if (t > 3 * 1000 * 1000)
			break;
		speaker->process(t);
		threadSleep(1000);
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
		for (const SpeakerDevice &d : list->devices())
		{
			CAGE_LOG(SeverityEnum::Info, "listing", "-------------------------------------");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "id: '" + d.id + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "name: '" + d.name + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "group: '" + d.group + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "vendor: '" + d.vendor + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "channels: " + d.channels);
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", Stringizer() + "sample rate min: " + d.minSamplerate + ", max: " + d.maxSamplerate + ", default: " + d.defaultSamplerate);
			if (d.available)
			{
				testDevice(d.id, 44100);
				testDevice(d.id, 48000);
			}
			else
				CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", "device not available");
		}

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
		return 1;
	}
}
