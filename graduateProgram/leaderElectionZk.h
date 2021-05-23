#ifndef _LEADERELECTIONZK_
#define  _LEADERELECTIONZK_


#include "leaderElection.h"
#include "logSystem.h"
#include "define.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include "zkAdapter.h"

class leaderElectionZk : public leaderElection
{
public:
	leaderElectionZk();
	leaderElectionZk(std::string localIP, int& is_register);
	leaderElectionZk(std::string, zkAdapter* ,int&);

	~leaderElectionZk();
	//void electionLeader(bool &is_leader, watcher_fn fn);
	void electionLeader(watcher_fn fn, void* watcherCtx);
	void getLeader(std::string& leaderIP);
	string getLeader();
	void watchLeader(std::string localIP, std::string& localPath, watcher_fn fn , void* watcherCtx);
	//bool createServices();
	//void leaderChange(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
private:
	logSystem* log;
	zkAdapter* zk;
	std::string local_ip;
	std::string leader_ip;
};

#endif //_LEADERELECTIONZK_