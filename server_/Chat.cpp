#include "Chat.hpp"
#include "Utility.h"

using LOG = logger::FileLogger::e_logType;

Chat::Chat() {
	log = logger::FileLogger::getInstance();
}

void Chat::addClient(const std::shared_ptr<ClientListener>& client) {

		std::string msg = "\\login " + std::to_string(client->getClientId());
		{
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String(msg.c_str()));
		}
		{
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String("Welcome to the Awesome Chat Server!"));
		}

		*log << LOG::LOG_INFO << type << "Client #" + std::to_string(client->getClientId()) + " is added to chat";
		idToClient[client->getClientId()] = client;
		if (!clientContainer->getMessageHistory().empty()) {
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String(createMessageFromQueue(clientContainer->getMessageHistory()).c_str()));
		}
}

void Chat::popWaiting() {
	if (!waitingClient.empty()) {
		clientContainer->popWaiting();
		addClient(waitingClient.front());
		waitingClient.pop();
	}
}

void Chat::SetTcpserver(IServerObserver* server) {
	tcpserver = server;
}

void Chat::setClientContainer(ClientContainer* clients) {
	clientContainer = clients;
}

void Chat::removeClientById(v_int64 id) {
	*log << LOG::LOG_INFO << type << "Client #" + std::to_string(id) + " is disconnected";
	if (idToClient.count(id) != 0) {
		idToClient.erase(id);
	}
}

bool Chat::isClienAdded(v_int64 id) {
	return idToClient.count(id) != 0;
}

std::shared_ptr<ClientListener> Chat::getClientById(v_int64 id) {
	if (idToClient.count(id) != 0) {
		return idToClient[id];
	}
	return nullptr;
}

void Chat::sendMessageToAllAsync(v_int64 source_id, const oatpp::String& message) {
	clientContainer->updateMessageHistory(message->std_str());
	tcpserver->sendToAll(message.get()->std_str());
	std::lock_guard<std::mutex> m(m_writeMessage);
	for (auto& pair : idToClient) {
		if (pair.first != source_id) {
			pair.second->sendMessageAsync(message);
		}
	}
	*log << LOG::LOG_MESSAGE << type << message.get()->std_str();
}

void Chat::sendMessageToAllAsync(const oatpp::String& message) {
	std::lock_guard<std::mutex> m(m_writeMessage);
	for (auto& pair : idToClient) {
		pair.second->sendMessageAsync(message);
	}
}

std::atomic<v_int32> Chat::SOCKETS(0); 

void Chat::onAfterCreate_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {
	std::this_thread::sleep_for(std::chrono::milliseconds(700));
	int id;
	bool isAdded = clientContainer->addSocket(type, id);
	*log << LOG::LOG_INFO << type << "Ñlient #" + std::to_string(id) + " is connected";
	auto client = std::make_shared<ClientListener>(socket, this, id);
	socket->setListener(client);
	if (isAdded) {
		addClient(client);
	}
	else {
		waitingClient.push(client);
		{
			std::lock_guard<std::mutex> m(m_writeMessage);
			client->sendMessageAsync(oatpp::String("The maximum number of messenger clients was reached. Please wait..."));
		}
		*log << LOG::LOG_INFO << type << "Ñlient #" + std::to_string(id) + " is added to waiting";
	}
}
void Chat::onBeforeDestroy_NonBlocking(const std::shared_ptr<ClientListener::AsyncWebSocket>& socket) {
	auto client = std::static_pointer_cast<ClientListener>(socket->getListener());
	removeClientById(client->getClientId());
	client->invalidateSocket();
	if (clientContainer->deleteSocket(type)) {
		popWaiting();
	}
	else {
		tcpserver->popWaiting();
	}
}