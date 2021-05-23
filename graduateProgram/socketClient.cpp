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
		if (WSAStartup(sockVersion, &wsaData) != 0) //WSAStartup����0��ʾ���ó�ʼ���ɹ�
		{
			throw "�׽��ֳ�ʼ��ʧ��";
		}

		/*�����׽���*/
		//AF_INET��ʾIPv4��SOCK_STREAM���ݴ��䷽ʽ��IPPROTO_TCP����Э��;
		client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (client_socket == INVALID_SOCKET)
		{
			throw "�׽��ִ���ʧ��";
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(INTERNAL_HOST);
		serAddr.sin_addr.S_un.S_addr = inet_addr(leader_ip.c_str());
		if (connect(client_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //����ʧ��   
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

void socketClient::internalSend(string & data, int& res)  //res��¼send�Ƿ��ͳɹ�
{

	const char * sendData;
	sendData = data.c_str();

	res = send(client_socket, sendData, strlen(sendData), 0);   //send�ɹ�������֤���ݱ��ɹ����뷢�ͻ�����



	//send()������������ָ����socket�����Է�����  
	//int send(int s, const void * msg, int len, unsigned int flags)  
	//sΪ�ѽ��������ӵ�socket��msgָ���������ݣ�len��Ϊ���ݳ��ȣ�����flagsһ����0  
	//�ɹ��򷵻�ʵ�ʴ��ͳ�ȥ���ַ�����ʧ�ܷ���-1������ԭ�����error   
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
	{  //����ʧ��   
		log->writeToCache("connect node error !");
		closesocket(node_socket);
		isconnected = false;
		return;
	}
	isconnected = true;
}