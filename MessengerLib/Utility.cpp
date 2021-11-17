#include "Utility.h"
#include <sstream> 
#include <iomanip>

Result ReadTextFile(const std::string& path, std::string& result) {
	std::ifstream file(path);
	if (file.is_open()) {
		std::string buf;
		buf.reserve(100);
		result.reserve(100);
		while (std::getline(file, buf)) {
			result += buf;
		}
		file.close();
		return Result::Success;
	}
	else {
		return Result::Error;
	}
}

Configure ParseJsonToConfig(const std::string& json) {
	rapidjson::Document d;
	d.Parse(json.c_str());

	assert(d.HasMember("n"));
	assert(d["n"].IsInt());
	assert(d.HasMember("m"));
	assert(d["m"].IsInt());
	assert(d.HasMember("loggerlines"));
	assert(d["loggerlines"].IsInt());
	assert(d.HasMember("tcphost"));
	assert(d["tcphost"].IsString());
	assert(d.HasMember("tcpport"));
	assert(d["tcpport"].IsInt());
	assert(d.HasMember("wshost"));
	assert(d["wshost"].IsString());
	assert(d.HasMember("wsport"));
	assert(d["wsport"].IsInt());
	auto s = d["tcphost"].GetString();

	Configure conf(d["n"].GetInt(), d["m"].GetInt(), d["loggerlines"].GetInt()
		, d["tcphost"].GetString(), d["tcpport"].GetInt()
		, d["wshost"].GetString(), d["wsport"].GetInt());
	return conf;
}

Configure SetConfig(const std::string& path) {
	std::string json;
	if (ReadTextFile(path, json) == Result::Success) {
		Configure config(ParseJsonToConfig(json));
		return config;
	}
	return Configure();
}

void ParseMessage(const std::string& input, std::string& command, std::string& message) {
	if (input.empty()) return;
	size_t count = input.find(' ');

	command = input.substr(1, count - 1);
	message = input.substr(count + 1);
}

Commands GetCommand(std::string command) {
	auto it = COMMANDS.find(command);
	if (it != COMMANDS.end()) {
		return it->second;
	}
	else {
		return Commands::NOT_FOUND;
	}
}

std::string genMessage(const std::string login, const std::string& message) {
	std::stringstream ss;
	ss << "Client [ " << std::setw(2) << login << "] : " + message;
	return ss.str();
}

std::string createMessageFromQueue(const std::deque<std::string>& deq) {
	std::string res;
	for (const auto& d : deq) {
		res += d + "\n";
	}
	res.pop_back();
	return res;
}