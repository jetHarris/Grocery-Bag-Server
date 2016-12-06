#pragma once


#include "ConnectedPackage.hpp"

#pragma comment (lib, "Ws2_32.lib")

class TCPServerSocket {
	SOCKET _s;
	WSADATA _w;
	TCPSocketError _error;
	int _intError;
	int _port;
	std::vector<ConnectedPackage*> _activeSockets;
	bool _closedCalled;

	bool startWSA() {
		int error = WSAStartup(0x0202, &_w);
		if (error) {
			_intError = error;
			_error = TCPSocketError::WSAStartupFailure;
			return false;
		}

		if (_w.wVersion != 0x0202){
			WSACleanup();
			_error = TCPSocketError::WSAVersionError;
			_intError = -1;
			return false;
		}
		return true;
	}

public:

	TCPServerSocket(int port) :_closedCalled(false), _port(port), _intError(0), _error(TCPSocketError::NoError), _activeSockets() {
		if (!startWSA())
			return;

		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(_port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_s == INVALID_SOCKET) {
			_intError = -2;
			_error = TCPSocketError::InvalidSocket;
			return;
		}
		int err = bind(_s, (LPSOCKADDR)&addr, sizeof(addr));
		if (err == SOCKET_ERROR) {
			_intError = -3;
			_error = TCPSocketError::SocketError;
			return;
		}

		if (listen(_s, SOMAXCONN)) {
			_intError = -4;
			_error = TCPSocketError::ListenError;
			return;
		}

	}

	ConnectedPackage& acceptClient() {
		if (hasError())
			throw "Socket in Error! Use getError() for more information...";

		struct sockaddr_storage their_addr;
		int addr_size = sizeof(their_addr);
		SOCKET s = accept(_s, (struct sockaddr*)&their_addr, &addr_size);
		ConnectedPackage* cp = new ConnectedPackage(s, their_addr, INVALID_SOCKET != s);
		_activeSockets.push_back(cp);
		return *cp;
	}


	TCPSocketError getError() { return _error; }
	int getErrorNumber() { return _intError; }
	int getPort() { return _port; }
	
	bool hasError() { return _intError != 0; }

	void close() {
		if (!_closedCalled) {
			_closedCalled = true;			//ensure everything is called only once
			for (auto& s : _activeSockets)
				s->close();
			shutdown(_s, SD_BOTH);
			if (_s)
				closesocket(_s);
		}
	}

	~TCPServerSocket() {
		close();
		for (auto& s : _activeSockets)
			delete s;
		
		WSACleanup();
		
	}
};