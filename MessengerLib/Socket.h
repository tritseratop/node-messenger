#pragma once
#include "Result.h"
#include "Endpoint.h"
#include "Constants.h"

// TODO overload operator ==

class Socket {
public:
	Socket(SOCKET handle = INVALID_SOCKET);
	Result Close();
	Result Create();
	Result Bind(Endpoint endpoint);
	Result Listen(Endpoint endpoint, int backlog = MAX_CLIENT_COUNT);
	Result Listen(std::string ip = IP, int port = PORT);
	Result Accept(Socket& outSocket);
	Result Connect(Endpoint endpoint);
	bool IsConnected();
	Result Send(const void* data, int numberOfBytes, int& bytesSent);
	Result Recv(void* data, int numberOfBytes, int& bytesSent);
	Result Send(const std::string& message);
	Result Recv(std::string& message);
	SOCKET GetSocketHandle() const;
	void setLogin(const std::string& login_);
	std::string getLogin();
private:
	SOCKET handle = INVALID_SOCKET;
	std::string login;
	bool isConnected = false;
};