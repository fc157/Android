#ifndef _TASKASSIGNMENT_
#define _TASKASSIGNMENT_


#include <unordered_map>
#include <vector>
#include <string>
#include "define.h"
#include <algorithm>


bool cmp(nodeInfo a, nodeInfo b);


class taskAssignment
{
public:
	taskAssignment();
	~taskAssignment();
	void assignmentStrategy(std::vector<nodeInfo>& nodeMap);
	void insertsort(std::vector<nodeInfo> &a, int start, int gap);
	void assignmentNode(task& my_task, nodeInfo& my_node, std::vector<nodeInfo>& nodeMap);
	void calculateResource(task& my_task, float& need_resrouce);
	//void compute();  //计算所有资源信息，返回最优结点
};

#endif // !_TASKASSIGNMENT_