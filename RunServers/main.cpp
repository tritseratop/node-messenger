#include "TCPServer.h"
#include "WebsockServer.h"
#include "Utility.h"
#include <napi.h>

Napi::Value Run(const Napi::CallbackInfo& info) {
	//Configure configure(SetConfig("D:/Develop/network cpp_/vs2019/messenger_oatpp/config.json"));
	Configure configure;
	oatpp::base::Environment::init();
	auto log = logger::FileLogger::getInstance(10, "Messenger Logger", "logger.txt");
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