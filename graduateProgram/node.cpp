#include "node.h"

void acceptMessageCallBack(string & task_info, void* watcherCtx)
{
	node* my_node = (node*)watcherCtx;
	my_node->callbackForAcceptMessage(task_info);
}

void leaderChangeCallBack(zhandle_t * zh, int type, int state, const char * path, void * watcherCtx)
{
	node* my_node = (node*)watcherCtx;
	my_node->electionLeader();
}
void node::electionLeader()
{
	if (is_register_inzoo == ZOK || is_register_inzoo == ZNODEEXISTS)
	{
		leader_election->electionLeader(leaderChangeCallBack, (void *)this);
	}
	if (leader_election->getLeader() == node_info->local_ip)
	{
		resource_manager = new resourceManager(zk, socket_client);
	}
}
node::node()
{
	std::string path = LOG_ROOT_PATH;
	log = new logSystem(path.append("/node/"));

	node_info = new nodeInfo();
	resource_adapter = new windowsResourceAdapter(*node_info);
	zk = new zkAdapter();

	boost::thread* check_zoo = new boost::thread(&node::ifExistInZoo, boost::ref(*this));
	check_zoo->detach();

	leader_election = new leaderElectionZk(node_info->local_ip, zk ,is_register_inzoo);

	socket_server = new socketServer(node_info->local_ip);
	socket_client = new socketClient(node_info->local_ip, leader_election->getLeader());

	boost::thread* acceptThread = new boost::thread(&node::acceptMessage, boost::ref(*this));
	boost::thread* sendThread = new boost::thread(&node::sendMessageAlarm, boost::ref(*this));

	electionLeader();

	
	acceptThread->detach();
	sendThread->detach();

	
	work = true;
}

node::~node()
{
	delete log;
	delete node_info;
	delete resource_adapter;
	delete zk;
	delete leader_election;
	delete socket_server;
	delete socket_client;
}

//bool node::registerNode()
//{
//	return true;
//}

bool node::isLeader()
{
	return leader_election->getLeader() == node_info->local_ip;
}


void node::nodeIsRegisteredInLeader()
{
	string data;
	nodeInfo resource_rate;
	

	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	
	while (work)
	{
		while (!send_ready)
			send_condition.wait(lock);

		resource_rate.local_ip = node_info->local_ip;
		resource_adapter->getWinCpuUsage(resource_rate.cpu_rate);

		resource_adapter->getWinFreeMem(resource_rate.free_mem);

		resource_rate.free_mem = resource_rate.free_mem * 0.9; //剩余10%当任务执行需要的内存大于开始计算的内存，再进行申请
		serialization(resource_rate, data);

		int res = -1;
		
		socket_client->internalSend(data, res);
		if (res == -1)
		{
			work = false;
		}
	}
}


void node::callbackForNoticeLoseNode(string& ip) //通知与zk失联的节点，失联节点收到后会与zk重新连接
{
	
}

void node::sendMessageAlarm()
{
	while (work)
	{
		//正常一秒唤醒一次。
		send_ready = true;
		send_condition.notify_one();
		Sleep(1000);
	}
}

int node::isRegisterInZoo()
{
	return is_register_inzoo;
}

void node::acceptMessage()
{
	//string res;
	
	func_accept_message func = acceptMessageCallBack;
	socket_server->acceptMessage(func, (void*)this);
	
}

void node::ifExistInZoo()
{
	string path = BASE_PATH;
	path.append("/" + node_info->local_ip);
	int res = -1;
	while (work)
	{
		zk->nodeIfExist(path, res);
		if (res != ZOK)
		{
			work = false;
			zk->connectZk();
			work = true;
		}
		Sleep(2000);
	}
}


void node::serialization(nodeInfo & resource_rate, string &res)
{
	res += "1";
	res += "ip:";
	res += resource_rate.local_ip;
	res += "'";

	res += "cpu_rate:";
	res += resource_rate.cpu_rate;
	res += "'";

	res += "free_mem:";
	res += resource_rate.free_mem;
	res += "'";
}

void node::deserialization(string s, nodeInfo& res)
{ 
		string key = "";
		for (int i = 1; i < s.length(); i++)
		{
			if (s[i] == ':')
			{
				i++;
				//int j = i;
				int count = 0;
				while (i < s.length() && s[i] != '\'')
				{
					count++;
				}
				string value(s.substr(i, count));
				i += count;
				if (key == "ip")
				{
					res.local_ip = value;
				}
				else if (key == "cpu_rate")
				{
					res.cpu_rate = std::stof(value);
				}
				else if (key == "free_mem")
				{
					res.free_mem = std::stof(value);
				}
				else
				{
					log->writeToCache("nodeInfo deserialization error");
					exit(1);
				}
				key = "";
			}
			else
			{
				key += s[i];
			}

		}
		//messagetype = 1;
}

void node::deserialization(string s, string ip, bool& res, string& task_message)
{
	string key = "";
	for (int i = 1; i < s.length(); i++)
	{
		if (s[i] == ':')
		{
			i++;
			//int j = i;
			int count = 0;
			while (i < s.length() && s[i] != '\'')
			{
				count++;
			}
			string value(s.substr(i, count));
			i += count;
			if (key == "ip")
			{
				ip = value;
			}
			else if (key == "result")
			{
				if (value == "ok")
				{
					res = true;
				}
			}
			else if (key == "task")
			{
				task_message = value;
			}
			else
			{
				log->writeToCache("task_message deserialization error");
				exit(1);
			}
			key = "";
		}
		else
		{
			key += s[i];
		}

	}
}

bool node::getWork()
{
	return work;
}



void node::callbackForAcceptMessage(string & task_info)
{
	nodeInfo deser_res;
	if (task_info[0] == '1') //心跳信息
	{
		deserialization(task_info, deser_res);
		//void (*func)(string& ip) = &this->callbackForNoticeLoseNode;
		resource_manager->acceptMessage(deser_res);
	}
	else if (task_info[0] == '2') //集群中某个节点收到客户端发来的任务，发送leader来进行分配。
	{
		if (leader_election->getLeader() == node_info->local_ip)
		{
			task my_task;
			string task_string(task_info.begin() + 1, task_info.end());
			my_task.stringToTask(task_string, my_task);
			resource_manager->addTaskToQueue(my_task);
		}
		else
		{
			//将任务发给leader
		}
		
		
	}
	else if (task_info[0] == '3')//leader分配给其他结点的任务信息
	{
		send_ready = true;
		send_condition.notify_one();
	}
	else if (task_info[0] == '4') //任务执行反馈信息，反馈给leader
	{
		string ip = "";
		bool res = false;
		string task_message = "";
		deserialization(task_info, ip, res, task_message);
		if (res == true && ip != "")
		{
			task my_task;
			my_task.stringToTask(task_message, my_task);
			resource_manager->delTaskOfNode(ip, my_task);
		}
	}
	else if (task_info[0] == '5')
	{
		//非leader节点收到客户端任务，需要发送给leader
	}
}


