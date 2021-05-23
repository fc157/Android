#ifndef _SOCKETCLIENT_
#define _SOCKETCLIENT_

#include <string>
#include "define.h"
#include <WinSock2.h>
#include "logSystem.h"
#pragma comment(lib,"ws2_32.lib") 

class socketClient
{
public:
	socketClient(string & local_ip, string& leader_ip);
	~socketClient();
	void initCient(string& local_ip);
	void internalSend(string & data);
	void internalSend(string & data, int& res);
	void connectedNode(string node_ip, bool& isconnected, SOCKET& node_socket);
private:
	logSystem* log;

	SOCKET client_socket;
	string leader_ip;
};
#endif
