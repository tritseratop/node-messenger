#ifndef ClientListener_hpp
#define ClientListener_hpp

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/async/Lock.hpp"

class Chat;

// ������� ������� ������������ ����������
class ClientListener : public oatpp::websocket::AsyncWebSocket::Listener {
private:
	static constexpr const char* TAG = "Server_ClientListener";
private:
	std::shared_ptr<AsyncWebSocket> m_socket;
	Chat* m_chat;
	v_int64 clientId;
	oatpp::data::stream::ChunkedBuffer m_messageBuffer;
	oatpp::async::Lock m_writeLock;
private:
	OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, m_serverConnectionProvider);
public:
	ClientListener(const std::shared_ptr<AsyncWebSocket>& socket, 
		Chat* chat, 
		v_int64 id)
		: m_socket(socket)
		, m_chat(chat)
		, clientId(id)
	{}
	v_int64 getClientId();
	CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) override;
	CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) override;
	CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) override;
	CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
	void sendMessageAsync(const oatpp::String& message);
	void invalidateSocket();
};

#endif // !ClientListener_hpp
