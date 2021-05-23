#include "logSystem.h"


logSystem::logSystem(std::string& path)
{
	cacheIndex = 0;
	diskindex = 0;
	logCache = new std::string[LOG_CACHE_MAX];
	path_ = path;
	if (0 != _access(path_.c_str(), 0))
	{
		_mkdir(path_.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	fopen_s(&fileOut, (path_ + getCurrentTime()).c_str(), "a+");
}

logSystem::logSystem()
{
}


bool logSystem::writeToCache(std::string s)
{
	if (cacheIndex >= LOG_CACHE_MAX)
	{
		writeToDisk();
	}
	logCache[cacheIndex] = s + '\n';
	cacheIndex++;
	return true;
}

void logSystem::writeToDisk()
{
	for (int i = 0; i < LOG_CACHE_MAX; i++)
	{
		fwrite(logCache[i].c_str(), sizeof(char), logCache[i].length(), fileOut);
		logCache[i].clear();
	}
	diskindex += LOG_CACHE_MAX;
	if (diskindex >= LOG_DISK_MAX)
	{
		fclose(fileOut);
		fileOut = NULL;
		fopen_s(&fileOut, (path_ + getCurrentTime()).c_str(), "a+");
		diskindex = 0;
	}
	cacheIndex = 0;
}

string logSystem::getCurrentTime()
{
	time_t tt = time(NULL);
	tm t;
	localtime_s(&t, &tt);
	return std::to_string(t.tm_year + 1900) + "��" +
		std::to_string(t.tm_mon + 1) + "��" +
		std::to_string(t.tm_mday) + "�գ�" +
		std::to_string(t.tm_hour) + "��" +
		std::to_string(t.tm_min) + "��" +
		std::to_string(t.tm_sec) + "��";
}

logSystem::~logSystem()
{
	if (cacheIndex > 0)
	{
		writeToDisk();
	}
	fclose(fileOut);
	delete[]logCache;
}
