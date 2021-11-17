#pragma once

#include <string>
#include <map>

const int MAX_CLIENT_COUNT = 2;
const int MAX_MESSAGE_BUF_COUNT = 5;
const int PORT = 1111;
const char* const IP = "127.0.0.1";

class Configure {
public:
	Configure(int N = 3, int M = 5, int loggerlines = 10
		, const std::string& tcphost = "127.0.0.1", int tcpport = 1111
		, const std::string& wshost = "localhost", int wsport = 8080)
		: MAX_CLIENT_COUNT(N)
		, MAX_MESSAGE_BUF_COUNT(M)
		, MAX_LOGGER_LINES(loggerlines)
		, TCP_HOST(tcphost)
		, TCP_PORT(tcpport)
		, WS_HOST(wshost.c_str())
		, WS_PORT(wsport)
	{}

	/*Configure(const Configure& config)
		: MAX_CLIENT_COUNT(config.MAX_CLIENT_COUNT)
		, MAX_MESSAGE_BUF_COUNT(config.MAX_MESSAGE_BUF_COUNT)
		, TCP_HOST(config.TCP_HOST)
		, TCP_PORT(config.TCP_PORT)
		, WS_HOST(config.WS_HOST)
		, WS_PORT(config.WS_PORT)
	{}*/

	/*Configure& operator= (const Configure& config) {
		if (this == &config) {
			return *this;
		}
		MAX_CLIENT_COUNT = config.getMaxClientCount();
		MAX_MESSAGE_BUF_COUNT = config.getMaxMessageBufCount();
		PORT = config.getPort();
		IP = config.getIp();
		return *this;
	}*/

public:
	int MAX_CLIENT_COUNT;
	int MAX_MESSAGE_BUF_COUNT;
	std::string TCP_HOST;
	int TCP_PORT;
	std::string WS_HOST;
	int WS_PORT;
	int MAX_LOGGER_LINES;
};

enum Commands {
	LOGIN,
	WAIT,
	EXIT,
	NOT_FOUND
};

const std::map<std::string, Commands> COMMANDS = { {"login", Commands::LOGIN}, {"exit", Commands::EXIT}, {"wait", Commands::WAIT}};