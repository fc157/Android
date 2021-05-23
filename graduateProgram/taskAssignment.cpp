#include "taskAssignment.h"

taskAssignment::taskAssignment()
{
}

taskAssignment::~taskAssignment()
{
}

bool cmp(nodeInfo a, nodeInfo b)
//按内存从小到大，cpu利用率从小到大排序。
{
	if (a.free_mem == b.free_mem)
	{
		return a.cpu_rate < b.cpu_rate;
	}
	else
	{
		return a.free_mem < b.free_mem;
	}
}

void taskAssignment::insertsort(std::vector<nodeInfo> &a, int start, int gap)
{
	int i, j;
	unsigned long free_mem_tmp;
	float cpu_rate_tmp;
	for (int k = start; k < a.size() - gap; k = k + gap)
	{
		i = k;
		j = i + gap;
		free_mem_tmp = a[j].free_mem;
		cpu_rate_tmp = a[j].cpu_rate;
		while ((free_mem_tmp < a[i].free_mem) || (free_mem_tmp == a[i].free_mem && cpu_rate_tmp < a[i].cpu_rate))
		{
			a[j] = a[i];
			j = j - gap;
			i = i - gap;
			if (i < 0) break;
		}
		a[j].cpu_rate = cpu_rate_tmp;
		a[j].free_mem = free_mem_tmp;
	}
}

void taskAssignment::assignmentStrategy(std::vector<nodeInfo>& nodeMap )  //对node进行排序
{
	int gap = nodeMap.size() / 2;
	while (gap > 0)
	{
		for (int i = 0; i < gap; i++)
		{
			insertsort(nodeMap, i, gap);
		}
		gap = gap / 2;
	}
}

void taskAssignment::assignmentNode(task & my_task, nodeInfo & my_node, std::vector<nodeInfo>& nodeMap)
{
	float need_resrouce = 0;
	calculateResource(my_task, need_resrouce);

	for (auto it = 0; it < nodeMap.size(); it++)
	{
		if (nodeMap[it].free_mem < need_resrouce)
		{
			continue;
		}
		my_node = nodeMap[it];
		break;
		//socket->sendMessage(socketMap[it->first]);
	}
}

void taskAssignment::calculateResource(task & my_task, float & need_resrouce)
{
}


