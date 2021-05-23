#ifndef _ZKADAPTER_
#define _ZKADAPTER_

#include <zookeeper.h>
#include <zookeeper_log.h>
#include "logSystem.h"
#include "define.h"


#define zk_hosts_ "127.0.0.1:8090"
#define ZK_MAX_CONNECT_TIMES 3

void initWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx);

class zkAdapter
{
public:
	zkAdapter();
	~zkAdapter();
	void connectZk();
	//void nodeChange(zhandle_t *zh, int type,int state, const char *path, void *watcherCtx);
	
	void CreateNode(string& localIP, int& is_register);

	void getChildrenNode(String_vector&);
	void wgetChildrenNode(String_vector& childrenPath, watcher_fn fn, void* watcherCtx);
	void getData(string path, string& res, Stat* stat);
	void setData(string path, string & res, int & len, Stat & stat);

	void nodeIfExist(string& path, int& res);

	void setWatcher(string path, void* watcherCtx, Stat& stat, watcher_fn* fn, bool& res);
private:
	zhandle_t* zhandle_;
	int timeout_ = 5000;
	logSystem* my_log;
	bool connected_ = false;
};

#endif // !_ZKADAPTER_