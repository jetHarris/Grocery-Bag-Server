#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
#include <WS2tcpip.h>



enum TCPSocketError {
	WSAStartupFailure,
	WSAVersionError,
	InvalidSocket,
	SocketError,
	ListenError,
	NoError
};

class ConnectedPackage {
	sockaddr_storage _client_addr;
	SOCKET _connectedSocket;
	int _clientPort;
	std::string _clientIP;
	TCPSocketError _error;
	int _intError;
	bool _closedCalled;
	bool _validConnection;

public:
	ConnectedPackage(SOCKET s, sockaddr_storage client_addr, bool valid):_closedCalled(false), _connectedSocket(s),_validConnection(valid),
		_client_addr(client_addr), _error(TCPSocketError::NoError),_intError(0) {
		struct sockaddr_in* client_info = (struct sockaddr_in*)&_client_addr;
		char str[INET_ADDRSTRLEN];
		IN_ADDR ipAddr = client_info->sin_addr;
		inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
		
		_clientIP = std::string(str);
		_clientPort = client_info->sin_port;
		
	}
	
	TCPSocketError getError() { return _error; }
	int getErrorNumber() { return _intError; }
	bool hasError() { return _intError != 0; }

	int sendData(std::vector<char>& data) {
		int bytesSent = send(_connectedSocket, data.data(), data.size(), 0);
		if (SOCKET_ERROR == bytesSent) {
			_intError = WSAGetLastError();
			_error = TCPSocketError::SocketError;
		}
		return bytesSent;
	}

	//getData
	//gets the data from the client using recv
	//Accepts: -
	//Returns: a vector of char
	//Modifies: -
	//Note: THIS METHOD PERFORMS A BLOCKING CALL
	std::vector<char> getData() {
		const unsigned bufferSize = 1024;
		std::vector<char> data;
		char buffer[bufferSize];
		memset(buffer, 0, sizeof(buffer));

		int res = 0;
		res = recv(_connectedSocket, buffer, bufferSize - 1, 0);
		if (res > 0)
		{
			for (int i = 0; i < res; ++i)
				data.push_back(buffer[i]);
		}
		return data;
	}

	int getClientPort() { return _clientPort; }
	std::string getClientIP() { return _clientIP; }

	void close() {
		if (!_closedCalled) {
			_closedCalled = true;
			
			if (_connectedSocket) {
				shutdown(_connectedSocket, SD_BOTH);
				closesocket(_connectedSocket);
			}
		}
	}

	~ConnectedPackage() {
		close();
	}

	bool isValid() { return _validConnection; }
	ConnectedPackage(ConnectedPackage const& rhs) = delete;
	ConnectedPackage& operator =(ConnectedPackage const& rhs) = delete;
};