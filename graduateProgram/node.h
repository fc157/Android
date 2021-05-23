#ifndef _NODE_
#define _NODE_

#include "resourceAdapter.h"
#include "windowsResourceAdapter.h"
#include <string>
#include "zkAdapter.h"
#include "leaderElection.h"
#include "resourceManager.h"
#include <list>
#include "task.h"
#include <queue>
#include <boost/thread/thread.hpp>
#include "leaderElectionZk.h"
#include "socketClient.h"
#include "socketServer.h"



class node
{
public:
	node();
	~node();
	//bool registerNode();
	bool isLeader();
	void nodeIsRegisteredInLeader(); //心跳检测
	//void leaderChange(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
	void electionLeader();
	void acceptMessage();
	void ifExistInZoo();
	void serialization(nodeInfo & resource_rate, string &res);
	void deserialization(string s, nodeInfo& res);
	void deserialization(string s, string ip, bool& res, string& task_message);
	bool getWork();
	void callbackForAcceptMessage(string& res);

	void callbackForNoticeLoseNode(string& ip);
	void sendMessageAlarm();

	int isRegisterInZoo();
	//std::string getLocalIP();
private:
	nodeInfo* node_info = NULL;
	bool is_leader = false;

	int is_register_inzoo = -1;  //是否在zookeeper上创建结点成功
	bool work = false;
	socketServer* socket_server;
	socketClient* socket_client;
	leaderElection* leader_election;
	resourceAdapter* resource_adapter;
	resourceManager* resource_manager;

	std::condition_variable send_condition;
	bool send_ready = false;

	std::condition_variable check_if_inzoo_condition;

	std::condition_variable accept_condition;
	logSystem* log;
	zkAdapter* zk;

};



#endif // !_NODE_