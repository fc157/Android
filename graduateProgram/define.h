#ifndef _DEFINE_
#define _DEFINE_




#define LOG_ROOT_PATH "E:/graduateProgram/log"
#define LOG_CACHE_MAX 1000
#define LOG_DISK_MAX (LOG_CACHE_MAX*10)   

#define TRY_CREATE_NOTE_MAX_COUNT 3

#define INTERNAL_HOST 8080

#define BASE_PATH "/node"

#include <list>
#include "task.h"
#include <string>

struct nodeInfo
{
	std::string local_ip;
	float cpu_rate;
	unsigned long free_mem;

	//bool operator < (const nodeInfo &a) const
	//{
	//	if (a.free_mem == free_mem)
	//	{
	//		return cpu_rate < a.cpu_rate;  //cpu利用率从小到大
	//	}
	//	else
	//	{
	//		return free_mem < a.free_mem; //空闲内存从小到大
	//	}

	//}
	//std::list<task> task_list;
};

//struct resourceRate
//{
//
//	std::string local_ip;
//	float cpu_rate;
//	float memory_rate;
//	//std::list<task> task_list;
//};

typedef void (*func_accept_message)(std::string& res, void* watcherCtx);

//struct EqualKey
//{
//	bool operator () (const resourceRate &lhs, const resourceRate &rhs) const
//	{
//		return lhs.local_ip == rhs.local_ip
//			&& lhs.memory_rate == rhs.memory_rate
//			&& lhs.cpu_rate == rhs.cpu_rate;
//	}
//};
//
//struct HashFunc
//{
//	std::size_t operator()(const resourceRate &key) const
//	{
//		using std::size_t;
//		using std::hash;
//
//		return ((hash<string>()(key.local_ip)
//			^ (hash<float>()(key.cpu_rate) << 1)) >> 1)
//			^ (hash<float>()(key.memory_rate) << 1);
//	}
//};


#endif // !_DEFINE_

