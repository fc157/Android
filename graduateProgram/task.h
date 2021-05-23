#ifndef _TASK_
#define _TASK_

#include <string>
class task
{
private:
	std::string data;
public:
	static void taskToString(std::string& task_info, task& mt_task);  //将task类型转换为string
	static void stringToTask(std::string& task_info, task& mt_task); //将string类型转换为task
	//bool ifEqual();
	bool operator==(task& a1);
};

#endif //_TASK_

