#include "TCPClient.h"

#include <thread>
#include <future>
#include <iostream>

Napi::Value Client::Initialize(const Napi::CallbackInfo& info) {
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0) {
		std::cerr << "Failed to start up the winsock API." << std::endl;
		return Napi::Boolean::New(info.Env(), false);
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		std::cerr << "Could not find a usable version of the winsock API dll." << std::endl;
		return Napi::Boolean::New(info.Env(), false);
	}
	return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Client::Shutdown(const Napi::CallbackInfo& info) {
	WSACleanup();
	return Napi::Boolean::New(info.Env(), true);
}

Napi::Value Client::Create(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Create());
}

Napi::Value Client::Close(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::Boolean::New(env, main_socket.Close());
}

Result Client::Connect(Endpoint endpoint) {
	return main_socket.Connect(endpoint);
}

Napi::Value Client::Connect(const Napi::CallbackInfo& info) {
	return Napi::Boolean::New(info.Env(), main_socket.Connect(Endpoint(config.TCP_HOST.c_str(), config.TCP_PORT)));
}

Result Client::defineLogin() {
	std::string message = "Enter your login: ";
	while (true) {
		std::cout << message;
		std::string login;
		std::getline(std::cin, login);
		if (login.empty()) { continue; }
		if (main_socket.Send(login) == Result::Error) {
			int error = WSAGetLastError();
			return Result::Error;
		}
		std::string response;
		if (main_socket.Recv(response) == Result::Error) {
			int error = WSAGetLastError();
			return Result::Error;
		}
		if (response == login) { 
			return Result::Success; 
		}
		else {
			message = "This login has already used.\nPlease, enter new login: ";
		}
	}
}

Result Client::setLogin(std::string login_) {
	login = login_;
	return Result::Success;
}

std::thread native_thread;
Napi::ThreadSafeFunction tsfn;
// ÆÈÅÑÒÜ
Napi::Value Client::StartChating(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	if (info.Length() < 1) {
		std::cout << "Expected one argument" << std::endl;
		return  Napi::Boolean::New(info.Env(), false);
	}
	else if (!info[0].IsFunction()) {
		std::cout << "Expected  arg to be function" << std::endl;
		return  Napi::Boolean::New(info.Env(), false);
	}
	Napi::Function output = info[0].As<Napi::Function>();
	tsfn = Napi::ThreadSafeFunction::New(
		env,							// Environment
		info[0].As<Napi::Function>(),	// JS function from caller
		"TSFN",							// Resource name
		0,								// Max queue size (0 = unlimited).
		1,								// Initial thread count
		[](Napi::Env) {   // Finalizer used to clean threads up
		}
	);
	native_thread = std::thread(
		[this]() {
			auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string* message) {
				// Transform native data into JS data, passing it to the provided
				// `jsCallback` -- the TSFN's JavaScript function.
				jsCallback.Call({ Napi::String::New(env, *message) });
				delete message;
			};
			while (true) {
				std::string* message = new std::string();
				if (main_socket.Recv(*message) == Result::Error) {
					int error = WSAGetLastError();
					return;
				}
				if ((*message)[0] == '\\') {
					handleCommand(*message);
				}
				else {
					//std::cout << message << std::endl;
					napi_status status = tsfn.BlockingCall(message, callback);
					if (status != napi_ok) {
						std::cout << "Napi::ThreadSafeNapi::Function.BlockingCall() failed" << std::endl;
					}
				}
			}
			tsfn.Release();
		}
	);
	native_thread.detach();
	return Napi::Boolean::New(info.Env(), false);
}

Napi::Value Client::Send(const Napi::CallbackInfo& info) {
	std::string send_msg = std::string(info[0].As<Napi::String>());
	if (login.empty()) { 
		return Napi::Boolean::New(info.Env(), true);
	}
	if (main_socket.Send(genMessage(login, send_msg)) == Result::Error) {
		int error = WSAGetLastError();
		return Napi::Boolean::New(info.Env(), false);
	}
	return Napi::Boolean::New(info.Env(), true);
}

void Client::handleCommand(std::string msg) {
	std::string command, login;
	ParseMessage(msg, command, login);
	switch (GetCommand(command)) {
	case Commands::LOGIN:
		setLogin(login);
		break;
	default:
		return;
	}
}

Napi::Value Client::CreateNewItem(const Napi::CallbackInfo& info) {
	Napi::FunctionReference* constructor =
		info.Env().GetInstanceData<Napi::FunctionReference>();
	return constructor->New({ });
}

Napi::Object Client::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Client", {
		InstanceMethod<&Client::Initialize>("Initialize"),
		InstanceMethod<&Client::Shutdown>("Shutdown"),
		InstanceMethod<&Client::Create>("Create"),
		InstanceMethod<&Client::Close>("Close"),
		InstanceMethod<&Client::Connect>("Connect"),
		InstanceMethod<&Client::StartChating>("StartChating"),
		InstanceMethod<&Client::Send>("Send"),
		});
	Napi::FunctionReference* constructor = new Napi::FunctionReference();

	*constructor = Napi::Persistent(func);
	exports.Set("Client", func);

	env.SetInstanceData<Napi::FunctionReference>(constructor);
	return exports;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	Client::Init(env, exports);
	return exports;
}

NODE_API_MODULE(client, Init)