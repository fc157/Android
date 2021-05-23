#include "leaderElectionZk.h"


typedef boost::shared_lock<boost::shared_mutex> readLock;
typedef boost::unique_lock<boost::shared_mutex> writeLock;


boost::shared_mutex rwmutex;



leaderElectionZk::leaderElectionZk()
{
}

leaderElectionZk::leaderElectionZk(string localIP, int& is_register)
{
	std::string logPath = LOG_ROOT_PATH;
	log = new logSystem(logPath.append("leaderElectionZk/"));
	zk = new zkAdapter();


	zk->CreateNode(localIP, is_register);

	local_ip = localIP;
}

leaderElectionZk::leaderElectionZk(string localIP, zkAdapter * zk, int & is_register)
{
	std::string logPath = LOG_ROOT_PATH;
	log = new logSystem(logPath.append("/leaderElectionZk/"));
	this->zk = zk;

	zk->CreateNode(localIP, is_register);
	local_ip = localIP;
}

leaderElectionZk::~leaderElectionZk()
{
	delete log;
	//delete zk;
}

//void leaderElectionZk::electionLeader(bool &is_leader, watcher_fn* fn)
//{
//	String_vector* childrenPath;
//	zk->getChildrenNode(childrenPath);
//	std::sort(childrenPath->data[0], childrenPath->data[childrenPath->count-1]);
//
//	int len;
//	zk->getData(childrenPath->data[0], NULL, leader_ip, len, NULL, NULL);
//	string localPath = "/node/" + local_ip;
//	if (leader_ip == local_ip)
//	{
//		is_leader = true;
//	}
//	else
//	{
//		watchLeader(childrenPath->data[0], localPath, fn);
//	}
//}


void leaderElectionZk::electionLeader( watcher_fn fn , void* watcherCtx)
{
	String_vector childrenPath;
	zk->getChildrenNode(childrenPath);
	std::sort(childrenPath.data[0], childrenPath.data[childrenPath.count - 1]);

	string path = BASE_PATH ;
	path.append("/");
	path.append(childrenPath.data[0]);
	
	zk->getData(path, leader_ip, NULL );  //最先注册的节点为leader
	string localPath = "/node/" + local_ip;
	log->writeToCache("leader is:" + leader_ip);

	if (leader_ip != local_ip)
	{
		watchLeader(childrenPath.data[0], localPath, fn ,watcherCtx);
	}
}

void leaderElectionZk::watchLeader(string leaderPath,string& localPath, watcher_fn fn, void* watcherCtx)
{
	Stat stat;
	bool res;
	//对leader设置监听，leader结点发生变化时，调用leaderChange函数

	zk->setWatcher(leaderPath, NULL, stat, &fn, res);
	if (res == ZOK)
	{
		log->writeToCache(localPath+"设置监听成功");
	}
	else if (res == ZNONODE)
	{
		log->writeToCache("leader节点不存在,导致监听失败");
		electionLeader(fn , watcherCtx);
	}
	else
	{
		log->writeToCache("未知错误,导致监听失败");
		exit(1);
	}
}

//void leaderElectionZk::leaderChange(zhandle_t * zh, int type, int state, const char * path, void * watcherCtx)
//{
//	//electionLeader();
//}



void leaderElectionZk::getLeader(string& leaderIP)
{
	leaderIP = leader_ip;
}

string leaderElectionZk::getLeader()
{
	return leader_ip;
}


