#include <cage-core/logger.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>

#include <cage-engine/speakerList.h>
#include <cage-engine/speaker.h>

#include <cmath>

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
		real sample = std::sin(angle);
		for (uint32 ch = 0; ch < data.channels; ch++)
			data.buffer[sampleIndex * data.channels + ch] = sample.value;
	}
}

void testDevice(const string &deviceId, uint32 sampleRate)
{
	CAGE_LOG(SeverityEnum::Info, "listing", "-----------------");

	SpeakerCreateConfig cnf;
	cnf.deviceId = deviceId;
	cnf.sampleRate = sampleRate;
	Holder<Speaker> speaker = newSpeaker(cnf, Delegate<void(const SoundCallbackData &)>().bind<&synthesizeTone>());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "channels: " + speaker->channels());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "sample rate: " + speaker->sampleRate());
	CAGE_LOG(SeverityEnum::Info, "speaker", stringizer() + "latency: " + speaker->latency());
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
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "id: '" + d.id + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "name: '" + d.name + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "group: '" + d.group + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "vendor: '" + d.vendor + "'");
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "channels: " + d.channels);
			CAGE_LOG_CONTINUE(SeverityEnum::Info, "listing", stringizer() + "sample rate min: " + d.minSamplerate + ", max: " + d.maxSamplerate + ", default: " + d.defaultSamplerate);
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
