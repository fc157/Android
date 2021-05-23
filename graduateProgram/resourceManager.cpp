#include "resourceManager.h"


void zkCallBack(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
//����ʱ���ص�·���Ǽ�����·��������������Ǹ�·�������ظ�·����������嵽�ڵ�·��
{
	resourceManager* res_manager = (resourceManager*)watcherCtx;
	if (type == ZOO_CREATED_EVENT)
	{
		res_manager->initNodeMap();
	}
	else if (ZOO_DELETED_EVENT)
	{
		res_manager->deleteNodeOfMap();
	}
}


resourceManager::resourceManager()
{
	zk = new zkAdapter();

	string log_path = LOG_ROOT_PATH;
	log_path.append("resourceManager/");
	log = new logSystem(log_path);
}

//resourceManager::resourceManager(socketClient* socket)
//{
//	this->socket = socket;
//	task_assignment = new taskAssignment();
//	boost::thread* assignTaskThread = new boost::thread(&resourceManager::assignmentNode, boost::ref(*this));
//	//assignTaskThread->join();
//}

resourceManager::resourceManager(zkAdapter * zk, socketClient* socket)
{
	string log_path = LOG_ROOT_PATH;
	log_path.append("/resourceManager/");
	log = new logSystem(log_path);

	this->zk = zk;

	this->socket = socket;

	task_assignment = new taskAssignment();

	initNodeMap();

	boost::thread* assignTaskThread = new boost::thread(&resourceManager::assignmentNode, boost::ref(*this));
}

void resourceManager::initNodeMap()
{
	String_vector child_vector;
	zk->wgetChildrenNode(child_vector, zkCallBack, (void*)this);
	string path = BASE_PATH;
	nodeInfo null_node;
	null_node.local_ip = "";
	null_node.cpu_rate = 0;
	null_node.free_mem = 0;
	string data = "";
	for (int i = 0; i < child_vector.count; i++)
	{
		string tmp = child_vector.data[i];
		path.append("/" +  tmp);
		zk->getData(path, data, NULL);
		null_node.local_ip = data;
		if (nodeIfInMap(null_node.local_ip) == false)
		{
			nodeMap.push_back(null_node);
			bool if_connected = false;
			connectNode(null_node.local_ip, if_connected);
		}
	}
	task_assignment->assignmentStrategy(nodeMap);
}

void resourceManager::setWatcher()
{
}

void resourceManager::connectNode(string node_ip, bool& if_connected)
{
	SOCKET node_socket;
	//bool if_connected = false;
	socket->connectedNode(node_ip, if_connected, node_socket);
	if (if_connected == true && socketMap.find(node_ip) == socketMap.end())
	{
		socketMap[node_ip] = node_socket;
	}
}

bool resourceManager::nodeIfInMap(nodeInfo & my_node , int& position)
{
	for (int i = 0; i < nodeMap.size(); i++)
	{
		if (nodeMap[i].local_ip == my_node.local_ip)
		{
			position = i;
			return true;
		}
	}
	return false;
}

bool resourceManager::nodeIfInMap(char* my_node)
{
	for (int i = 0; i < nodeMap.size(); i++)
	{
		if (nodeMap[i].local_ip == my_node)
		{
			return true;
		}
	}
	return false;
}

bool resourceManager::nodeIfInMap(string& my_node)
{
	for (int i = 0; i < nodeMap.size(); i++)
	{
		if (nodeMap[i].local_ip == my_node)
		{
			return true;
		}
	}
	return false;
}

void resourceManager::checkNodeMap()
{

}

void resourceManager::deleteNodeOfMap() //ɾ��map�е��ߵĽڵ�
{
	String_vector child_vector;
	zk->wgetChildrenNode(child_vector, zkCallBack, (void*)this);

	for (int it = 0; it < nodeMap.size(); it++)
	{
		bool res = false;
		for (int i = 0; i < child_vector.count; i++)
		{
			if (nodeMap[it].local_ip == child_vector.data[i])
			{
				res = true;
			}
		}
		if (res == false)
		{
			if (socketMap.find(nodeMap[it].local_ip) != socketMap.end())
			{
				socketMap.erase(nodeMap[it].local_ip);
			}
			else
			{
				log->writeToCache("deleteNodeOfMap ��ɾ����ӦsocketMap���Ҳ�������node");
			}
			nodeTaskSet.erase(nodeMap[it].local_ip);
			nodeMap.erase(nodeMap.begin()+it);
			break;
		}
	}
}

void resourceManager::deleteNodeOfMap(string & ip)
{
	for (int i = 0; i < nodeMap.size(); i++)
	{
		if (nodeMap[i].local_ip == ip)
		{
			if (socketMap.find(ip) != socketMap.end())
			{
				socketMap.erase(ip);
			}
			nodeMap.erase(nodeMap.begin() + i);
			break;
		}
	}
}






void resourceManager::acceptMessage(nodeInfo & res) 
//����������Ϣ��������nodeMap��û�иýڵ�ʱ������֤zk���Ƿ��иýڵ㣬���û�У�˵���ýڵ��zk�Ѿ�ʧ��
{
	int position = 0;
	if (nodeIfInMap(res , position) == true)
	{
		nodeMap[position] = res;
	}
	else
	{
		initNodeMap();
		if (nodeIfInMap(res, position) == true)
		{
			nodeMap[position] = res;
		}
		//else //�ýڵ��zk�Ѿ�ʧ����֪ͨ�ýڵ���������zk
		//{
		//	if (socketMap.find(res.local_ip) != socketMap.end())
		//	{
		//		string tmp = "please connect zookeeper";
		//		send(socketMap[res.local_ip], tmp.c_str(), tmp.length(), NULL);
		//	}
		//}
		
	}
	task_assignment->assignmentStrategy(nodeMap);
}

void resourceManager::assignmentNode() 
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	
	float need_resrouce = 0;
	nodeInfo my_node;
	my_node.cpu_rate = 0;
	my_node.free_mem = 0;
	
	if (nodeMap.empty() != true)
	{
		while (!assignment_ready)
		{
			assignmentTask.wait(lock);
		}
		
		while (!task_que.empty())
		{
			my_node.local_ip = "";
			string task_info = "";
			task my_task = task_que.front();
	
			task_assignment->assignmentNode(my_task, my_node, nodeMap);
			if (my_node.local_ip == "") //û�ҵ����ʵĽڵ�,�˴�Ҫ�Ķ����Ӹ��ڵ�������ϢҪ������������ڴ档
				//�����ǰ������Ҫ���ڴ�������нڵ������ڴ棬������ִ�в���
			{
				log->writeToCache("������Ҫ�ڴ���ڵ�ǰ�ڵ��ڴ�");
				task_que.pop();
				continue;
			}
			if (socketMap.find(my_node.local_ip) == socketMap.end())
			{
				initNodeMap();
			}
			if (socketMap.find(my_node.local_ip) == socketMap.end())
			{
				log->writeToCache("leader ���������ڵ�ʧ��");  //�ڵ�����ʧ�ܣ�����ýڵ㲻���ã��������Դ�zookeeper�ϻ�ȡ���нڵ㣬����Ƿ��иýڵ㡣
				deleteNodeOfMap(my_node.local_ip);
			}
			else
			{
				send(socketMap[my_node.local_ip], task_info.c_str(), task_info.length(), 0);
				nodeTaskSet[my_node.local_ip].push_back(my_task);
				break;
			}
			task_que.pop();
		}
		assignment_ready = false;
	}
}



void resourceManager::addTaskToQueue(task acceppt_task)
{
	task_que.push(acceppt_task);
	assignment_ready = true;
	assignmentTask.notify_one();
}

void resourceManager::delTaskOfNode(string & ip, task & task_message)
{
	for (int i = 0; i < nodeTaskSet[ip].size(); i++)
	{
		if (nodeTaskSet[ip][i] == task_message)
		{
			nodeTaskSet[ip].erase(nodeTaskSet[ip].begin()+i);
			break;
		}
	}
}



//void resourceManager::callbackForAcceptMessage(resourceRate& res)
//{
//	nodeMap[res.local_ip] = res;
//}

//resourceManager::resourceManager(resourceAdapter * resource_adapter, 
//	leaderElection * leader_election)
//{
//	string local_ip, leader_ip;
//	resource_adapter->getIP(local_ip);
//	leader_election->getLeader(leader_ip);
//	if (local_ip == leader_ip)
//	{
//
//	}
//	else 
//	{
//
//	}
//}
