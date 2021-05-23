#include "windowsResourceAdapter.h"


windowsResourceAdapter::windowsResourceAdapter()
{
}

windowsResourceAdapter::windowsResourceAdapter(nodeInfo& node_info)
{
	std::string log_path = LOG_ROOT_PATH;
	myLog = new logSystem(log_path.append("/resourceAdapter/"));
	getIP(node_info.local_ip);
}

windowsResourceAdapter::~windowsResourceAdapter()
{
	delete myLog;
}

void windowsResourceAdapter::getIP(string& localIP)
{
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return;

	char local[255] = { 0 };
	gethostname(local, sizeof(local));

	
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return;

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip
	localIP.assign(inet_ntoa(addr));
	
	WSACleanup();
}

void windowsResourceAdapter::getCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed)
{

	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//注册表子键路径
	CRegKey regkey;//定义注册表类对象
	LONG lResult;//LONG型变量－反应结果
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //打开注册表键
	if (lResult != ERROR_SUCCESS)
	{
		return;
	}
	WCHAR chCPUName[50] = { 0 };
	DWORD dwSize = 50;

	//获取ProcessorNameString字段值
	if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"), chCPUName, &dwSize))
	{
		chProcessorName = chCPUName;
	}

	//查询CPU主频
	DWORD dwValue;
	if (ERROR_SUCCESS == regkey.QueryDWORDValue(_T("~MHz"), dwValue))
	{
		dwMaxClockSpeed = dwValue;
	}
	regkey.Close();//关闭注册表
	//UpdateData(FALSE);

	//获取CPU核心数目
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);
	dwNum = si.dwNumberOfProcessors;

	switch (si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
	{
		chProcessorType.Format(_T("Intel 386 processor"));
	}
	break;
	case PROCESSOR_INTEL_486:
	{
		chProcessorType.Format(_T("Intel 486 Processor"));
	}
	break;
	case PROCESSOR_INTEL_PENTIUM:
	{
		chProcessorType.Format(_T("Intel Pentium Processor"));
	}
	break;
	case PROCESSOR_INTEL_IA64:
	{
		chProcessorType.Format(_T("Intel IA64 Processor"));
	}
	break;
	case PROCESSOR_AMD_X8664:
	{
		chProcessorType.Format(_T("AMD X8664 Processor"));
	}
	break;
	default:
		chProcessorType.Format(_T("未知"));
		break;
	}

	//GetDisplayName()
}

void  windowsResourceAdapter::getMemoryInfo(CString &dwTotalPhys, CString &dwTotalVirtual)
{
	//   TODO:     Add   extra   initialization   here 
	MEMORYSTATUS   Mem;
	//   get   the   memory   status 
	GlobalMemoryStatus(&Mem);

	DWORD dwSize = (DWORD)Mem.dwTotalPhys / (1024 * 1024);
	DWORD dwVirtSize = (DWORD)Mem.dwTotalVirtual / (1024 * 1024);

	dwTotalPhys.Format(_T("物理内存:%ld MB"), dwSize);
	dwTotalVirtual.Format(_T("虚拟内存:%ld MB"), dwVirtSize);
}

void windowsResourceAdapter::getDiskInfo(DWORD &dwNum, CString chDriveInfo[])
{
	DWORD DiskCount = 0;

	//利用GetLogicalDrives()函数可以获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据。
	DWORD DiskInfo = GetLogicalDrives();

	//通过循环操作查看每一位数据是否为1，如果为1则磁盘为真,如果为0则磁盘不存在。
	while (DiskInfo)
	{
		//通过位运算的逻辑与操作，判断是否为1
		Sleep(10);
		if (DiskInfo & 1)
		{
			DiskCount++;
		}
		DiskInfo = DiskInfo >> 1;//通过位运算的右移操作保证每循环一次所检查的位置向右移动一位。*/
	}

	if (dwNum < DiskCount)
	{
		return;//实际的磁盘数目大于dwNum
	}
	dwNum = DiskCount;//将磁盘分区数量保存


	//-------------------------------------------------------------------//
	//通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度
	int DSLength = GetLogicalDriveStrings(0, NULL);

	WCHAR* DStr = new WCHAR[DSLength];
	memset(DStr, 0, DSLength);

	//通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
	GetLogicalDriveStrings(DSLength, DStr);

	int DType;
	int si = 0;
	BOOL fResult;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;

	//读取各驱动器信息，由于DStr内部数据格式是A:\NULLB:\NULLC:\NULL，所以DSLength/4可以获得具体大循环范围
	for (int i = 0; i < DSLength / 4; ++i)
	{
		Sleep(10);
		CString strdriver = DStr + i * 4;
		CString strTmp, strTotalBytes, strFreeBytes;
		DType = GetDriveType(strdriver);//GetDriveType函数，可以获取驱动器类型，参数为驱动器的根目录
		switch (DType)
		{
		case DRIVE_FIXED:
		{
			strTmp.Format(_T("本地磁盘"));
		}
		break;
		case DRIVE_CDROM:
		{
			strTmp.Format(_T("DVD驱动器"));
		}
		break;
		case DRIVE_REMOVABLE:
		{
			strTmp.Format(_T("可移动磁盘"));
		}
		break;
		case DRIVE_REMOTE:
		{
			strTmp.Format(_T("网络磁盘"));
		}
		break;
		case DRIVE_RAMDISK:
		{
			strTmp.Format(_T("虚拟RAM磁盘"));
		}
		break;
		case DRIVE_UNKNOWN:
		{
			strTmp.Format(_T("虚拟RAM未知设备"));
		}
		break;
		default:
			strTmp.Format(_T("未知设备"));
			break;
		}

		//GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据
		fResult = GetDiskFreeSpaceEx(strdriver,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);

		if (fResult)
		{
			strTotalBytes.Format(_T("磁盘总容量%fMB"), (float)i64TotalBytes / 1024 / 1024);
			strFreeBytes.Format(_T("磁盘剩余空间%fMB"), (float)i64FreeBytesToCaller / 1024 / 1024);
		}
		else
		{
			strTotalBytes.Format(_T(""));
			strFreeBytes.Format(_T(""));
		}
		chDriveInfo[i] = strTmp + _T("(") + strdriver + _T("):") + strTotalBytes + strFreeBytes;
		si += 4;
	}
}

void windowsResourceAdapter::getWinCpuUsage(float& cpu_rate) 
{
	HANDLE h_event;
	BOOL res;
	FILETIME preidle_time;
	FILETIME prekernel_time;
	FILETIME preuser_time;
	FILETIME myidle_time;
	FILETIME mykernel_time;
	FILETIME myuser_time;

	res = GetSystemTimes(&myidle_time, &mykernel_time, &myuser_time);
	preidle_time = myidle_time;
	prekernel_time = mykernel_time;
	preuser_time = myuser_time;

	h_event = CreateEventA(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled  

	WaitForSingleObject(h_event, 1000);
	res = GetSystemTimes(&myidle_time, &mykernel_time, &myuser_time);

	__int64 idle = CompareFileTime(preidle_time, myidle_time);
	__int64 kernel = CompareFileTime(prekernel_time, mykernel_time);
	__int64 user = CompareFileTime(preuser_time, myuser_time);

	__int64 cpu = (kernel + user - idle) * 100 / (kernel + user);
	__int64 cpuidle = (idle) * 100 / (kernel + user);
	cpu_rate = cpu;
	
	preidle_time = myidle_time;
	prekernel_time = mykernel_time;
	preuser_time = myuser_time;
}

__int64 windowsResourceAdapter::CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

	return (b - a);
}

void windowsResourceAdapter::getWinMemUsage(int& mem_rate) {
	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	mem_rate = ms.dwMemoryLoad;
}

void windowsResourceAdapter::getWinFreeMem(unsigned long& free_mem) {

	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	free_mem = ms.dwAvailPhys / (1024 * 1024); //装换为以MB为单位
}
