#ifndef _LOGSYSTEM_
#define _LOGSYSTEM_

#include <vector>
#include <fstream>
#include <string>
#include "define.h"
#include <time.h>
#include <direct.h>
#include <io.h>
using std::string;

class logSystem
{
public:
	logSystem(std::string& path);
	logSystem();
	//bool writeToCache(std::string&);
	bool writeToCache(std::string);
	//void writeToDisk(std::string&);
	void writeToDisk();
	string getCurrentTime();
	~logSystem();
private:
	string* logCache;
	string path_;
	std::FILE* fileOut;
	int cacheIndex = 0;
	int diskindex = 0; 
};

#endif // !_LOGSYSTEM_