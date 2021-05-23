#ifndef _LEADERELECTION_
#define  _LEADERELECTION_

#include <string>
#include <zookeeper.h>

class leaderElection
{
public:
	virtual void getLeader(std::string& leaderIP) = 0;
	virtual std::string getLeader() = 0;
	virtual void watchLeader(std::string localIP, std::string& localPath, watcher_fn fn, void* watcherCtx ) = 0;
	//virtual void registerNode();
	//virtual void electionLeader(bool &is_leader, watcher_fn fn)= 0;
	virtual void electionLeader(watcher_fn fn, void* watcherCtx ) = 0;


	//virtual void registerNodeRessult()=0;
private:
};

#endif