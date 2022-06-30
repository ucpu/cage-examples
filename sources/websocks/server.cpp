#include <cage-core/config.h>
#include <cage-core/logger.h>
#include <cage-core/networkWebsocket.h>
#include <cage-core/concurrent.h>
#include <vector>
#include <algorithm>

using namespace cage;

Holder<WebsocketServer> server;
std::vector<Holder<WebsocketConnection>> connections;

void update()
{
	// detect new connections
	while (auto c = server->accept())
	{
		CAGE_LOG(SeverityEnum::Info, "connection", Stringizer() + c->address() + ":" + c->port());
		connections.push_back(std::move(c));
	}

	std::vector<Holder<PointerRange<const char>>> messages;

	// receive messages
	std::erase_if(connections, [&](const Holder<WebsocketConnection> &it) {
		try
		{
			while (it->size() > 0)
			{
				Holder<PointerRange<const char>> msg = it->readAll();
				CAGE_LOG(SeverityEnum::Info, "message", String(msg));
				messages.push_back(std::move(msg));
			}
			return false;
		}
		catch (const cage::Disconnected &)
		{
			return true; // erase the connection
		}
	});

	// send messages
	std::erase_if(connections, [&](const Holder<WebsocketConnection> &it) {
		try
		{
			for (const auto &msg : messages)
				it->write(msg);
			return false;
		}
		catch (const cage::Disconnected &)
		{
			return true; // erase the connection
		}
	});
}

int main(int argc, const char *args[])
{
	// log to console
	Holder<Logger> log1 = newLogger();
	log1->format.bind<logFormatConsole>();
	log1->output.bind<logOutputStdOut>();

	server = newWebsocketServer(8456);
	while (true)
	{
		update();
		threadSleep(100000);
	}

	return 0;
}
