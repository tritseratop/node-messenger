#include "WSClient.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"

#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/core/async/Executor.hpp"

#include <thread>
#include <iostream>

std::thread native_thread;

Napi::Value WSClient::run(const Napi::CallbackInfo& info) {

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
            native_thread.join();
        }
    );

    native_thread = std::thread(
        [this]() {
            AppComponent component(config);
            //component_ = &component;
            OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
            OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

            auto connector = oatpp::websocket::Connector::createShared(connectionProvider);

            executor->execute<ClientCoroutine>(connector, this);
            std::this_thread::sleep_for(std::chrono::milliseconds(700));
            /*oatpp::websocket::Config config;
            config.maskOutgoingMessages = true;
            config.readBufferSize = 64;
            ClientCoroutine::SOCKET->setConfig(config);*/
            //std::mutex socketWriteMutex;
            //executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, &socketWriteMutex);
            /*std::thread test_handler([&executor, this, &socketWriteMutex] {
                while (true) {
                    std::string msg;
                    std::cin >> msg;
                    msg = genMessage(login, msg);
                    std::lock_guard<std::mutex> lock(socketWriteMutex);
                    executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, oatpp::String(msg.c_str()));
                }
            });
            test_handler.join();*/
            executor->join();
        }
    );
    native_thread.detach();
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value WSClient::Send(const Napi::CallbackInfo& info) {
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    std::string msg = std::string(info[0].As<Napi::String>());
    std::string send_msg = genMessage(login, msg);
    std::mutex socketWriteMutex;
    std::lock_guard<std::mutex> lock(socketWriteMutex);
    executor->execute<ClientSenderCoroutine>(ClientCoroutine::SOCKET, oatpp::String(send_msg.c_str()));
    return Napi::Boolean::New(info.Env(), true);
}

void WSClient::setLogin(std::string login_) {
    login = login_;
}

Napi::Value WSClient::CreateNewItem(const Napi::CallbackInfo& info) {
    Napi::FunctionReference* constructor =
        info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({ });
}

Napi::Object WSClient::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "WSClient", {
        InstanceMethod<&WSClient::run>("run"),
        InstanceMethod<&WSClient::Send>("Send"),
        });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("WSClient", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);
    return exports;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    WSClient::Init(env, exports);
    return exports;
}

NODE_API_MODULE(wsclient, Init)