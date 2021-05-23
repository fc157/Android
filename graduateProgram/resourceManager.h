#ifndef _RESOURCEMANAGER_
#define _RESOURCEMANAGER_
 
#include <unordered_map>


#include <vector>

#include "zkAdapter.h"
#include "leaderElection.h"
#include "taskAssignment.h"
#include "define.h"
#include <queue>
#include <boost/thread/thread.hpp>
#include "socketClient.h"

//struct Cmp
//{
//	bool operator () (nodeInfo const &a, nodeInfo const &b) const
//		//���ڴ��С����cpu�����ʴ�С��������
//	{
//		if (a.free_mem == b.free_mem)
//		{
//			return a.cpu_rate < b.cpu_rate;
//		}
//		else
//		{
//			return a.free_mem < b.free_mem;
//		}
//	}
//};


class resourceManager 
{
public:
	resourceManager();
	resourceManager(socketClient* socket);
	resourceManager(zkAdapter * zk, socketClient* socket);
	//resourceManager(resourceAdapter* ressource_adapter, leaderElection* leader_election);
	 void acceptMessage(nodeInfo& res);

	 void assignmentNode();
	 //void calculateResource(string& res , float& need_resrouce);
	 void addTaskToQueue(task acceppt_task);
	 void delTaskOfNode(string& ip, task& task_message);

	 void initNodeMap();
	 void checkNodeMap();
	 void deleteNodeOfMap();
	 void deleteNodeOfMap(string& ip);
	 //void zkCallBack(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);

	 void setWatcher();
	 void connectNode(std::string, bool& if_connected);

	 bool nodeIfInMap(nodeInfo& my_node, int& position);
	 bool nodeIfInMap(char* my_node);
	 bool nodeIfInMap(string& my_node);
private:
	std::vector<nodeInfo> nodeMap;  //�Զ���������,
	std::unordered_map<std::string, std::vector<task>> nodeTaskSet;  //����ÿ���ڵ�������ִ�е�����
	std::unordered_map<std::string, SOCKET> socketMap;

	taskAssignment* task_assignment;

	socketClient* socket;

	std::queue<task> task_que;  //����Ҫ���������

	leaderElection* leader_election;
	zkAdapter* zk;
	logSystem* log;

	std::condition_variable assignmentTask;
	bool assignment_ready = false;
};

#endif //_RESOURCEMANAGER_