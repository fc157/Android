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
		if (WSAStartup(sockVersion, &wsaData) != 0) //WSAStartup����0��ʾ���ó�ʼ���ɹ�
		{
			throw "WSAStartup ����ʧ��";
		}

		/*�����׽���*/
		//AF_INET��ʾIPv4��SOCK_STREAM���ݴ��䷽ʽ��IPPROTO_TCP����Э��;
		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket == INVALID_SOCKET)
		{
			throw "�׽��ִ���ʧ��";
		}
		//���ü�����ַ�Ͷ˿�
		sockaddr_in addrListen;
		addrListen.sin_family = AF_INET;     //ָ��IP��ʽ
		addrListen.sin_port = htons(INTERNAL_HOST);   //�󶨶˿ں�
		addrListen.sin_addr.S_un.S_addr = INADDR_ANY;  //��ʾ�κ�IP 
		if (bind(listenSocket, (SOCKADDR*)&addrListen, sizeof(addrListen)) == SOCKET_ERROR)  //(SOCKADDR*)
		{
			throw "��ʧ��";
			closesocket(listenSocket);
			exit(1);
		}
		if (listen(listenSocket, 5) == SOCKET_ERROR)
		{
			throw "��������";
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
	SOCKET revSocket;  //��Ӧ��ʱ���������ӵ��׽��ֵľ��
	sockaddr_in remoteAddr;   //�������ӵ��������ϵĵ�ַ��Ϣ
	char revData[RECN_MAX_LEN];

	int remoteAddrLen = sizeof(remoteAddr);
	/*�ȴ��ͻ������󣬷�������������*/

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
				revSocket = accept(listenSocket, (SOCKADDR*)&remoteAddr, &remoteAddrLen);  //�ȴ��ͻ��˽��룬ֱ���пͻ�����������Ϊֹ
				if (revSocket == INVALID_SOCKET)
				{
					log->writeToCache("�ͻ��˷������󣬷�������������ʧ��");
					continue;
				}
				string tmp = inet_ntoa(remoteAddr.sin_addr);
				log->writeToCache("���������տͻ���" + tmp + "����ɹ���" );
				
				//��������    
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



