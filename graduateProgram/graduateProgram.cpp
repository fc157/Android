#include "graduateProgram.h"

graduateProgram::graduateProgram()
{
	
}

graduateProgram::~graduateProgram()
{
	delete my_node;
}

//void graduateProgram::registerService(const string& name, IServices* service)
//{
//	if (servicesMap.find(name) == servicesMap.end())
//	{
//		servicesMap[name] = service;
//	}
//	else
//	{
//		std::cout << name <<" service has existed" << std::endl;
//	}
//}

void graduateProgram::start()  
{
	my_node = new node();
	while (my_node->getWork())
	{

	}
}


//bool graduateProgram::createServices()
//{
	/*for (auto it = servicesMap.begin(); it != servicesMap.end(); it++)
	{
		if (!it->second->createServices())
		{
			std::cout << it->first << ": createServices failed" << std::endl;
			return false;
		}
	}
	return true;*/
//}