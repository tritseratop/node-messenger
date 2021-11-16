#pragma once
#include "IncludeMe.h"
#include "ClientCommutator.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <napi.h>

class Client : public IClient, public Napi::ObjectWrap<Client> {
public:
	Client(const Napi::CallbackInfo& info) 
		: Napi::ObjectWrap<Client>(info)
		, config(Configure())
	{}
	static Napi::Object Init(Napi::Env, Napi::Object exports);
	static Napi::Value CreateNewItem(const Napi::CallbackInfo& info);

	Napi::Value Initialize(const Napi::CallbackInfo& info);
	Napi::Value Shutdown(const Napi::CallbackInfo& info);
	Napi::Value Create(const Napi::CallbackInfo& info);
	Napi::Value Close(const Napi::CallbackInfo& info);

	Result Connect(Endpoint endpoint);

	Napi::Value Connect(const Napi::CallbackInfo& info);
	Napi::Value StartChating(const Napi::CallbackInfo& info);
	Napi::Value Send(const Napi::CallbackInfo& info);

	Result setLogin(std::string login_);
	Result defineLogin();
private:
	void handleCommand(std::string msg);
	Socket main_socket;
	std::string login;
	size_t client_count = 0;
	const Configure config;
};
