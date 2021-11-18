#ifndef WSClient_hpp
#define WSClient_hpp

#include "WSListener.hpp"
#include "AppComponent.hpp"

#include <napi.h>

class WSClient : public Napi::ObjectWrap<WSClient> {
private:
	std::string login;
public:
	Napi::ThreadSafeFunction tsfn;
	WSClient(const Napi::CallbackInfo& info)
		: Napi::ObjectWrap<WSClient>(info)
	{
		oatpp::base::Environment::init();
	}
	~WSClient() {
		oatpp::base::Environment::destroy();
	}
	static Napi::Object Init(Napi::Env, Napi::Object exports);
	static Napi::Value CreateNewItem(const Napi::CallbackInfo& info);

	Napi::Value run(const Napi::CallbackInfo& info);
	Napi::Value Send(const Napi::CallbackInfo& info);
	void setLogin(std::string login_);
};

#endif //WSClient_hpp 

