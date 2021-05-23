#include "socketServer.h"


#define RECN_MAX_LEN 256


socketServer::socketServer(string & local_ip, string leader_ip)
{
	std::string path = LOG_ROOT_PATH;
	log = new logSystem(path.append("/socketInternal/"));
	initServer(local_ip);
}

socketServer::socketServer()
{
}

socketServer::socketServer(string & local_ip)
{
	std::string path = LOG_ROOT_PATH;
	log = new logSystem(path.append("socketServer/"));
	initServer(local_ip);
}


socketServer::~socketServer()
{
	WSACleanup();
	delete log;
}

void socketServer::initServer(string & local_ip)
{
	try
	{
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0) //WSAStartup返回0表示设置初始化成功
		{
			throw "WSAStartup 调用失败";
		}

		/*创建套接字*/
		//AF_INET表示IPv4，SOCK_STREAM数据传输方式，IPPROTO_TCP传输协议;
		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket == INVALID_SOCKET)
		{
			throw "套接字创建失败";
		}
		//配置监听地址和端口
		sockaddr_in addrListen;
		addrListen.sin_family = AF_INET;     //指定IP格式
		addrListen.sin_port = htons(INTERNAL_HOST);   //绑定端口号
		addrListen.sin_addr.S_un.S_addr = INADDR_ANY;  //表示任何IP 
		if (bind(listenSocket, (SOCKADDR*)&addrListen, sizeof(addrListen)) == SOCKET_ERROR)  //(SOCKADDR*)
		{
			throw "绑定失败";
			closesocket(listenSocket);
			exit(1);
		}
		if (listen(listenSocket, 5) == SOCKET_ERROR)
		{
			throw "监听出错";
			closesocket(listenSocket);
			exit(1);
		}
	}
	catch (const const char* msg)
	{
		log->writeToCache(msg);
		exit(1);
	}
	accept_work = true;
}


void socketServer::acceptMessage(func_accept_message func, void* watcherCtx)
{
	SOCKET revSocket;  //对应此时所建立连接的套接字的句柄
	sockaddr_in remoteAddr;   //接收连接到服务器上的地址信息
	char revData[RECN_MAX_LEN];

	int remoteAddrLen = sizeof(remoteAddr);
	/*等待客户端请求，服务器接收请求*/

	fd_set read_set, tmp_set;
	//FD_ZERO(&read_set);
	FD_ZERO(&tmp_set);

	//FD_SET(listenSocket, &read_set);
	FD_SET(listenSocket, &tmp_set);
	while (accept_work)
	{
		read_set = tmp_set;
		int select_res = select(listenSocket + 1, &read_set, NULL, NULL, NULL);
		if (select_res < 0)
		{
			log->writeToCache("select error");
			return;
		}
		else if (select_res > 0)
		{
			if (FD_ISSET(listenSocket, &read_set))
			{
				revSocket = accept(listenSocket, (SOCKADDR*)&remoteAddr, &remoteAddrLen);  //等待客户端接入，直到有客户端连接上来为止
				if (revSocket == INVALID_SOCKET)
				{
					log->writeToCache("客户端发出请求，服务器接收请求失败");
					continue;
				}
				string tmp = inet_ntoa(remoteAddr.sin_addr);
				log->writeToCache("服务器接收客户端" + tmp + "请求成功：" );
				
				//接收数据    
				int ret = recv(revSocket, revData, RECN_MAX_LEN, 0);
				string tmpData = revData;
				if (ret > 0)
				{			
					func(tmpData, watcherCtx);
				}
				FD_SET(revSocket, &tmp_set);
				
			}
			closesocket(revSocket);
		}
		
		
	}
	closesocket(listenSocket);
	
}




//void socketInternal::sendMessage(SOCKET send_socket)
//{
//	send(send_socket, message, strlen(message) + 1, 0);
//}



