#ifndef _SOCKETSERVER_
#define _SOCKETSERVER_

#include <string>
#include "define.h"
#include <WinSock2.h>
#include "logSystem.h"

#pragma comment(lib,"ws2_32.lib") 

using std::string;

class socketServer //端口号8090
{
public:
	socketServer();  
	socketServer(string & local_ip); //初始化服务端socket
	socketServer(string& local_ip, string leader_ip);
	~socketServer();
	void initServer(string& local_ip);
	
	void acceptMessage(func_accept_message func, void* watcherCtx);

	//void connectedLeader(string leader_ip, bool& isconnected);
	//void connectedNode(string node_ip, bool& isconnected, SOCKET& node_socket);
	//void sendMessage(SOCKET send_socket);
	//void serialization(resourceRate & resource_rate, string &res);
	//void deserialization(string s, resourceRate& res, bool& is_leader);
	//void sendToLeader();
private:
	SOCKET listenSocket;

	bool accept_work = false;
	logSystem* log;
};

#endif // !_SOCKETSERVER_