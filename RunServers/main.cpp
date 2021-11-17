#include "TCPServer.h"
#include "WebsockServer.h"
#include "Utility.h"
#include <napi.h>

Napi::Value Run(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	if (info.Length() < 1) {
		std::cout << "Expected one argument" << std::endl;
		return  Napi::Boolean::New(info.Env(), false);
	}
	else if (!info[0].IsString()) {
		std::cout << "Expected arg to be String" << std::endl;
		return  Napi::Boolean::New(info.Env(), false);
	}
	std::string path = std::string(info[0].As<Napi::String>());
	Configure configure(SetConfig(path));
	oatpp::base::Environment::init();
	auto log = logger::FileLogger::getInstance(100, "Messenger Logger", "logger.txt");
	ClientContainer clients(configure.MAX_CLIENT_COUNT);
	Server tcpserver(configure);
	WebsockServer websockserver(configure);
	tcpserver.setClientContainer(&clients);
	tcpserver.SetServer(&websockserver);
	
	std::thread tcp_thread([](Server& tcpserver) {
		tcpserver.Initialize();
		tcpserver.Create();
		tcpserver.StartListen();
		tcpserver.HandleClients();
	}, std::ref(tcpserver));

	websockserver.run(&tcpserver, &clients);

	oatpp::base::Environment::destroy();

	tcp_thread.join();
	return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(
        Napi::String::New(env, "run"),
        Napi::Function::New(env, Run)
    );
    return exports;
}

NODE_API_MODULE(server, Init)