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
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // �������ȡ��һ��ip
	localIP.assign(inet_ntoa(addr));
	
	WSACleanup();
}

void windowsResourceAdapter::getCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed)
{

	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//ע����Ӽ�·��
	CRegKey regkey;//����ע��������
	LONG lResult;//LONG�ͱ�������Ӧ���
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //��ע����
	if (lResult != ERROR_SUCCESS)
	{
		return;
	}
	WCHAR chCPUName[50] = { 0 };
	DWORD dwSize = 50;

	//��ȡProcessorNameString�ֶ�ֵ
	if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"), chCPUName, &dwSize))
	{
		chProcessorName = chCPUName;
	}

	//��ѯCPU��Ƶ
	DWORD dwValue;
	if (ERROR_SUCCESS == regkey.QueryDWORDValue(_T("~MHz"), dwValue))
	{
		dwMaxClockSpeed = dwValue;
	}
	regkey.Close();//�ر�ע���
	//UpdateData(FALSE);

	//��ȡCPU������Ŀ
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
		chProcessorType.Format(_T("δ֪"));
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

	dwTotalPhys.Format(_T("�����ڴ�:%ld MB"), dwSize);
	dwTotalVirtual.Format(_T("�����ڴ�:%ld MB"), dwVirtSize);
}

void windowsResourceAdapter::getDiskInfo(DWORD &dwNum, CString chDriveInfo[])
{
	DWORD DiskCount = 0;

	//����GetLogicalDrives()�������Ի�ȡϵͳ���߼����������������������ص���һ��32λ�޷����������ݡ�
	DWORD DiskInfo = GetLogicalDrives();

	//ͨ��ѭ�������鿴ÿһλ�����Ƿ�Ϊ1�����Ϊ1�����Ϊ��,���Ϊ0����̲����ڡ�
	while (DiskInfo)
	{
		//ͨ��λ������߼���������ж��Ƿ�Ϊ1
		Sleep(10);
		if (DiskInfo & 1)
		{
			DiskCount++;
		}
		DiskInfo = DiskInfo >> 1;//ͨ��λ��������Ʋ�����֤ÿѭ��һ��������λ�������ƶ�һλ��*/
	}

	if (dwNum < DiskCount)
	{
		return;//ʵ�ʵĴ�����Ŀ����dwNum
	}
	dwNum = DiskCount;//�����̷�����������


	//-------------------------------------------------------------------//
	//ͨ��GetLogicalDriveStrings()������ȡ�����������ַ�����Ϣ����
	int DSLength = GetLogicalDriveStrings(0, NULL);

	WCHAR* DStr = new WCHAR[DSLength];
	memset(DStr, 0, DSLength);

	//ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��
	GetLogicalDriveStrings(DSLength, DStr);

	int DType;
	int si = 0;
	BOOL fResult;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;

	//��ȡ����������Ϣ������DStr�ڲ����ݸ�ʽ��A:\NULLB:\NULLC:\NULL������DSLength/4���Ի�þ����ѭ����Χ
	for (int i = 0; i < DSLength / 4; ++i)
	{
		Sleep(10);
		CString strdriver = DStr + i * 4;
		CString strTmp, strTotalBytes, strFreeBytes;
		DType = GetDriveType(strdriver);//GetDriveType���������Ի�ȡ���������ͣ�����Ϊ�������ĸ�Ŀ¼
		switch (DType)
		{
		case DRIVE_FIXED:
		{
			strTmp.Format(_T("���ش���"));
		}
		break;
		case DRIVE_CDROM:
		{
			strTmp.Format(_T("DVD������"));
		}
		break;
		case DRIVE_REMOVABLE:
		{
			strTmp.Format(_T("���ƶ�����"));
		}
		break;
		case DRIVE_REMOTE:
		{
			strTmp.Format(_T("�������"));
		}
		break;
		case DRIVE_RAMDISK:
		{
			strTmp.Format(_T("����RAM����"));
		}
		break;
		case DRIVE_UNKNOWN:
		{
			strTmp.Format(_T("����RAMδ֪�豸"));
		}
		break;
		default:
			strTmp.Format(_T("δ֪�豸"));
			break;
		}

		//GetDiskFreeSpaceEx���������Ի�ȡ���������̵Ŀռ�״̬,�������ص��Ǹ�BOOL��������
		fResult = GetDiskFreeSpaceEx(strdriver,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);

		if (fResult)
		{
			strTotalBytes.Format(_T("����������%fMB"), (float)i64TotalBytes / 1024 / 1024);
			strFreeBytes.Format(_T("����ʣ��ռ�%fMB"), (float)i64FreeBytesToCaller / 1024 / 1024);
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

	h_event = CreateEventA(NULL, FALSE, FALSE, NULL); // ��ʼֵΪ nonsignaled ������ÿ�δ������Զ�����Ϊnonsignaled  

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
	free_mem = ms.dwAvailPhys / (1024 * 1024); //װ��Ϊ��MBΪ��λ
}
