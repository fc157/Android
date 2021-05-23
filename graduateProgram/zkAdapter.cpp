#include "zkAdapter.h"



void initWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx) 
{
	logSystem* my_log = (logSystem*)watcher_ctx;
	if (state == ZOO_CONNECTED_STATE) {
		//connected_ = true;
		my_log->writeToCache("InitWatcher() ZOO_CONNECTED_STATE");

	}
	else if (state == ZOO_AUTH_FAILED_STATE) {
		my_log->writeToCache("InitWatcher() ZOO_AUTH_FAILED_STATE");
	}
	else if (state == ZOO_EXPIRED_SESSION_STATE) {
		my_log->writeToCache("InitWatcher() ZOO_EXPIRED_SESSION_STATE");
	}
	else if (state == ZOO_CONNECTING_STATE) {
		my_log->writeToCache("InitWatcher() ZOO_CONNECTING_STATE");
	}
	else if (state == ZOO_ASSOCIATING_STATE) {
		my_log->writeToCache("InitWatcher() ZOO_ASSOCIATING_STATE");
	}
}

zkAdapter::zkAdapter()
{
	std::string zk_log_path = LOG_ROOT_PATH;
	my_log = new logSystem(zk_log_path.append("/zkAdapter/"));
	connectZk();
}

zkAdapter::~zkAdapter()
{
	delete my_log;
}


void zkAdapter::CreateNode(string& localIP, int& is_register)
{
	string path = BASE_PATH;
	int res = -1;
	nodeIfExist(path, res);
	if (res == ZNONODE)
	{
		zoo_create(zhandle_, path.c_str(), localIP.c_str(), localIP.length(), &ZOO_OPEN_ACL_UNSAFE, NULL, NULL, 0);
	}

	int i = 0;
	while ((is_register != ZOK && is_register != ZNODEEXISTS)&& i < TRY_CREATE_NOTE_MAX_COUNT)
	{
		time_t now_time;
		now_time = time(NULL);
		if (localIP != "")
		{
			path.append("/" + std::to_string(now_time));
		}
		is_register = zoo_create(zhandle_, path.c_str(), localIP.c_str(), localIP.length(), &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE | ZOO_EPHEMERAL, NULL, 0);
		i++;
		if (is_register != ZOK && is_register != ZNODEEXISTS)
		{
			Sleep(1000);
		}
	}
	if (i >= TRY_CREATE_NOTE_MAX_COUNT)
	{
		my_log->writeToCache("CreateNode failed");
		exit(1);
	}
}

void zkAdapter::getChildrenNode(String_vector& childrenPath) //最好是加个路径，代表要获取哪个路径下的节点
{
	zoo_get_children(zhandle_, BASE_PATH ,NULL, &childrenPath);
}

void zkAdapter::wgetChildrenNode(String_vector& childrenPath, watcher_fn fn, void* watcherCtx)
{
	zoo_wget_children(zhandle_, BASE_PATH, fn, watcherCtx, &childrenPath);
}

void zkAdapter::getData(string path , string& res , Stat* stat)
{
	int max_length = 512;
	char getbuf[512];
	memset(getbuf, 0, sizeof(getbuf));

	zoo_get(zhandle_, path.c_str(), 0, getbuf, &max_length, stat);
	
	res = getbuf;

}

void zkAdapter::setWatcher(string path, void* watcherCtx, Stat& stat, watcher_fn* fn, bool& res)
{
	//zoo_wget(zhandle_, path.c_str(), *fn, watcherCtx, const_cast<char*>(res.c_str()), &len, stat);
	res = zoo_wexists(zhandle_, path.c_str(), *fn, watcherCtx, &stat);
}

void zkAdapter::setData(string path, string & res, int & len, Stat & stat)
{
	zoo_set(zhandle_, path.c_str(), res.c_str(), len, stat.version);
}

void zkAdapter::nodeIfExist(string& path, int& res)
{
	Stat stat;
	res = zoo_wexists(zhandle_, path.c_str(), 0, NULL, &stat);

	if ( res != ZOK && res != ZNONODE )
	{
		my_log->writeToCache("nodeIfExist中zoo_exists函数调用失败:" + res);
		exit(1);
	}
}






void zkAdapter::connectZk()
{

	zhandle_ = zookeeper_init(zk_hosts_ , &initWatcher, timeout_, NULL, (void*)my_log, 0);

	if (zhandle_ == NULL) {
		my_log->writeToCache( "connect zookeeper over max!!!");
		exit(1);
	}
}

