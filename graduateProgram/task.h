#ifndef _TASK_
#define _TASK_

#include <string>
class task
{
private:
	std::string data;
public:
	static void taskToString(std::string& task_info, task& mt_task);  //��task����ת��Ϊstring
	static void stringToTask(std::string& task_info, task& mt_task); //��string����ת��Ϊtask
	//bool ifEqual();
	bool operator==(task& a1);
};

#endif //_TASK_

