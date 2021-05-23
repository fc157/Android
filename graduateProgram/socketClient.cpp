#include "socketClient.h"

socketClient::socketClient(string & local_ip, string& leader_ip)
{
	this->leader_ip = leader_ip;
	std::string path = LOG_ROOT_PATH;
	log = new logSystem(path.append("/socketClient/"));
	initCient(local_ip);
}

socketClient::~socketClient()
{
	WSACleanup();
	delete log;
}

void socketClient::initCient(string & local_ip)
{
	try
	{
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0) //WSAStartup返回0表示设置初始化成功
		{
			throw "套接字初始化失败";
		}

		/*创建套接字*/
		//AF_INET表示IPv4，SOCK_STREAM数据传输方式，IPPROTO_TCP传输协议;
		client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (client_socket == INVALID_SOCKET)
		{
			throw "套接字创建失败";
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(INTERNAL_HOST);
		serAddr.sin_addr.S_un.S_addr = inet_addr(leader_ip.c_str());
		if (connect(client_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //连接失败   
			log->writeToCache("connect leader error !");
			closesocket(client_socket);
			return;
		}
	}
	catch (const const char* msg)
	{
		log->writeToCache(msg);
		exit(1);
	}
}

void socketClient::internalSend(string & data)
{
	const char * sendData;
	sendData = data.c_str();
	send(client_socket, sendData, strlen(sendData), 0);
}

void socketClient::internalSend(string & data, int& res)  //res记录send是否发送成功
{

	const char * sendData;
	sendData = data.c_str();

	res = send(client_socket, sendData, strlen(sendData), 0);   //send成功仅仅保证数据被成功放入发送缓冲区



	//send()用来将数据由指定的socket传给对方主机  
	//int send(int s, const void * msg, int len, unsigned int flags)  
	//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0  
	//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error   
}


void socketClient::connectedNode(string node_ip, bool& isconnected, SOCKET& node_socket)
{

	node_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (node_socket == INVALID_SOCKET)
	{
		log->writeToCache("invalid socket!");
		return;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(INTERNAL_HOST);
	serAddr.sin_addr.S_un.S_addr = inet_addr(node_ip.c_str());
	if (connect(node_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  //连接失败   
		log->writeToCache("connect node error !");
		closesocket(node_socket);
		isconnected = false;
		return;
	}
	isconnected = true;
}