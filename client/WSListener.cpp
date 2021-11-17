#include "WSListener.hpp"
#include "WSClient.hpp"
#include <iostream>

void WSListener::handleCommand(std::string msg) {
    std::string command, login;
    ParseMessage(msg, command, login);
    switch (GetCommand(command)) {
    case Commands::LOGIN:
        client->setLogin(login);
        setLogin(login);
        break;
    default:
        return;
    }
}

void WSListener::setLogin(std::string login_) {
    login = login_;
}

std::string WSListener::getLogin() {
    return login;
}

oatpp::async::CoroutineStarter WSListener::onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onPing");
    return socket->sendPongAsync(message);
}

oatpp::async::CoroutineStarter WSListener::onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onPong");
    return nullptr;
}

oatpp::async::CoroutineStarter WSListener::onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onClose code=%d", code);
    return nullptr;
}

oatpp::async::CoroutineStarter WSListener::readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) {
    if (size == 0) { // message transfer finished
        auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string* message) {
            // Transform native data into JS data, passing it to the provided
            // `jsCallback` -- the TSFN's JavaScript function.
            jsCallback.Call({ Napi::String::New(env, *message) });
            delete message;
        };
        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.clear();
        if (wholeMessage->std_str()[0] == '\\') {
            handleCommand(wholeMessage->std_str());
        }
        else {
            std::string* message = new std::string();
            *message = wholeMessage->std_str(); //mb use memcpy
            napi_status status = client->tsfn.BlockingCall(message, callback);
            if (status != napi_ok) {
                std::cout << "Napi::ThreadSafeNapi::Function.BlockingCall() failed" << std::endl;
                delete message;
            }
        }
    }
    else if (size > 0) { // message frame received
        m_messageBuffer.writeSimple(data, size);
    }
    return nullptr;
}

std::shared_ptr<oatpp::websocket::AsyncWebSocket> ClientCoroutine::SOCKET(nullptr);

oatpp::async::Action ClientCoroutine::act() {
    return m_connector->connectAsync("ws").callbackTo(&ClientCoroutine::onConnected);
}

oatpp::async::Action ClientCoroutine::onConnected(const std::shared_ptr<oatpp::data::stream::IOStream>& connection) {
    m_socket = oatpp::websocket::AsyncWebSocket::createShared(connection, true);
    m_socket->setListener(std::make_shared<WSListener>(client));
    SOCKET = m_socket;
    return m_socket->listenAsync().next(yieldTo(&ClientCoroutine::onFinishListen));
}

oatpp::async::Action ClientCoroutine::onFinishListen() {
    return finish();
}