#ifndef _WINDOWSRESOURCEADAPTER_
#define _WINDOWSRESOURCEADAPTER_

//#include "windows.h"
#include "logSystem.h"

#include "resourceAdapter.h"
#include <string>
#include "define.h"
#include <atlbase.h>
#include "float.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
//#include "winsock2.h"
//#ifdef __cplusplus
//}
//#endif

//#define WINDOWS

class windowsResourceAdapter :public resourceAdapter
{
public:
	windowsResourceAdapter();
	windowsResourceAdapter(nodeInfo& node_info);
	~windowsResourceAdapter();

	void getIP(string &ip);

	void getMemoryInfo(CString &dwTotalPhys, CString &dwTotalVirtual);

	void getCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed);

	void getDiskInfo(DWORD &dwNum, CString chDriveInfo[]);

	__int64 CompareFileTime(FILETIME time1, FILETIME time2);
	
	void getWinCpuUsage(float&);

	void getWinMemUsage(int&);

	void getWinFreeMem(unsigned long& free_mem);
private:

	logSystem* myLog;
};

#endif // !_WINDOWSRESOURCEADAPTER_