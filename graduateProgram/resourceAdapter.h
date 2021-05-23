#ifndef _RESOURCEADAPTER_
#define _RESOURCEADAPTER_
//#include <afx.h>
#include <Afxtempl.h>
#include <string>

class resourceAdapter
{

public:
	virtual void getIP(std::string &ip) = 0;

	virtual void getMemoryInfo(CString &dwTotalPhys, CString &dwTotalVirtual) = 0;

	virtual void getCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed) = 0;
	
	virtual void getDiskInfo(DWORD &dwNum, CString chDriveInfo[]) = 0;
	virtual __int64 CompareFileTime(FILETIME time1, FILETIME time2) = 0;

	virtual void getWinCpuUsage(float&) = 0;

	virtual void getWinMemUsage(int&) = 0;
	virtual void getWinFreeMem(unsigned long& free_mem) = 0;
private:

};

#endif



