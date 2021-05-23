#ifndef _NODESTAYUS_
#define _NODESTAYUS_

#include <string>

#include "IServices.h"

struct Node
{
	std::string data;
	bool isleader;
};

class nodeStatus : public IServices
{
public:
	bool registerNode();
	bool isLeader();
	bool nodeIsRegister(); //�������
	
private:
	Node* node;
};

#endif // !_NODESTAYUS_