


#define _WIN32_DCOM

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#include "uuid.h"

#include "public.h"

#include "utils.h"

# pragma comment(lib, "wbemuuid.lib")


using namespace std;



int getWIndowsUUID(WCHAR * uuid,int size)
{
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return FALSE;  
	}

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation 
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities
		NULL                         // Reserved
	);
	if (FAILED(hres))
	{
		CoUninitialize();
		return FALSE; 
	}

	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator, (LPVOID*)&pLoc);
	if (FAILED(hres))
	{
		CoUninitialize();
		return FALSE;  
	}

	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices* pSvc = NULL;
	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object
		&pSvc                    // pointer to IWbemServices proxy
	);
	if (FAILED(hres))
	{
		pLoc->Release();
		CoUninitialize();
		return FALSE;  
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	// Set security levels on the proxy -------------------------
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to se
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities
	);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return FALSE;
	}

	// Use the IWbemServices pointer to make requests of WMI ----
	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_ComputerSystemProduct"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return FALSE;
	}

	IWbemClassObject* pclsObj;

	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
		if (0 == uReturn)
		{
			break;
		}
		VARIANT vtProp = { 0 };
		// Get the value of the Name property
		hr = pclsObj->Get(L"UUID", 0, &vtProp, 0, 0);
		//wcout << " OS Name : " << vtProp.bstrVal << endl;
		wcscpy(uuid, vtProp.bstrVal);
		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();

	CoUninitialize();

	return 0;
}



int getUUID() {
	int ret = 0;

	WCHAR wstruuid[256] = { 0 };

	ret = getWIndowsUUID(wstruuid, sizeof(wstruuid));

	char struuid[256];
	ret = wcstombs(struuid, wstruuid, sizeof(struuid));

	string uuid = string(struuid);

	removeChar(uuid, '-');

	lstrcpyA(g_uuid, uuid.c_str());
	g_uuid_len = lstrlenA(uuid.c_str());
	return TRUE;

	int uuidlen = uuid.size();

	int cnt = uuidlen / 16;
	int mod = uuidlen % 16;

	unsigned char* first = (unsigned char*)uuid.c_str();
	unsigned char* second = first + 16;
	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			first[j] = (first[j] + second[j]) / 2;
		}
		second += 16;
	}

	for (int j = 0; j < mod; j++)
	{
		first[j] = (first[j] + second[j]) / 2;
	}
	second += 16;

	memcpy(g_uuid, first, 16);

	g_uuid_len = 16;
	
	return TRUE;
}






#include <Windows.h>
#include <string>
#include <tchar.h>


typedef struct _dmi_header
{
	BYTE type;
	BYTE length;
	WORD handle;
}dmi_header;



typedef struct _RawSMBIOSData
{
	BYTE    Used20CallingMethod;
	BYTE    SMBIOSMajorVersion;
	BYTE    SMBIOSMinorVersion;
	BYTE    DmiRevision;
	DWORD   Length;
	BYTE    SMBIOSTableData[];
}RawSMBIOSData;


static void dmi_system_uuid(const BYTE* p, short ver)
{
	int only0xFF = 1, only0x00 = 1;
	int i;

	for (i = 0; i < 16 && (only0x00 || only0xFF); i++)
	{
		if (p[i] != 0x00) only0x00 = 0;
		if (p[i] != 0xFF) only0xFF = 0;
	}



	if (only0xFF)
	{
		printf("Not Present");
		return;
	}

	if (only0x00)
	{
		printf("Not Settable");
		return;
	}


	if (ver >= 0x0206)
		printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
			p[3], p[2], p[1], p[0], p[5], p[4], p[7], p[6],
			p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
	else
		printf("-%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
			p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
			p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
}


const char* dmi_string(const dmi_header* dm, BYTE s)
{
	char* bp = (char*)dm;
	size_t i, len;

	if (s == 0)
		return "Not Specified";

	bp += dm->length;

	while (s > 1 && *bp)
	{
		bp += strlen(bp);
		bp++;
		s--;
	}
	if (!*bp)
		return "BAD_INDEX";

	/* ASCII filtering */
	len = strlen(bp);
	for (i = 0; i < len; i++)
		if (bp[i] < 32 || bp[i] == 127)
			bp[i] = '.';
	return bp;
}



int getUUID2()
{
	DWORD bufsize = 0;
	BYTE buf[0x4000] = { 0 };
	int ret = 0;
	RawSMBIOSData* Smbios;
	dmi_header* h = NULL;
	int flag = 1;

	ret = GetSystemFirmwareTable('RSMB', 0, 0, 0);
	if (!ret)
	{
		printf("Function failed!\n");
		return 1;
	}

	printf("get buffer size is %d\n", ret);
	bufsize = ret;

	ret = GetSystemFirmwareTable('RSMB', 0, buf, bufsize);

	if (!ret)
	{
		printf("Function failed!\n");
		return 1;
	}



	Smbios = (RawSMBIOSData*)buf;
	BYTE* p = Smbios->SMBIOSTableData;

	if (Smbios->Length != bufsize - 8)
	{
		printf("Smbios length error\n");
		return 1;
	}

	for (int i = 0; i < Smbios->Length; i++) {
		h = (dmi_header*)p;

		if (h->type == 0 && flag) {
			printf("\nType %02d - [BIOS]\n", h->type);
			printf("\tBIOS Vendor : %s\n", dmi_string(h, p[0x4]));
			printf("\tBIOS Version: %s\n", dmi_string(h, p[0x5]));
			printf("\tRelease Date: %s\n", dmi_string(h, p[0x8]));

			if (p[0x16] != 0xff && p[0x17] != 0xff)
				printf("\tEC version: %d.%d\n", p[0x16], p[0x17]);
			flag = 0;
		}



		else if (h->type == 1) {
			printf("\nType %02d - [System Information]\n", h->type);
			printf("\tManufacturer: %s\n", dmi_string(h, p[0x4]));
			printf("\tProduct Name: %s\n", dmi_string(h, p[0x5]));
			printf("\tVersion: %s\n", dmi_string(h, p[0x6]));
			printf("\tSerial Number: %s\n", dmi_string(h, p[0x7]));
			printf("\tUUID: "); dmi_system_uuid(p + 0x8, Smbios->SMBIOSMajorVersion * 0x100 + Smbios->SMBIOSMinorVersion);
			printf("\tSKU Number: %s\n", dmi_string(h, p[0x19]));
			printf("\tFamily: %s\n", dmi_string(h, p[0x1a]));
		}
		p += h->length;
		while ((*(WORD*)p) != 0) p++;
		p += 2;
	}

	return 0;

}



int getVolumeSerialNum(DWORD* lpulVolumeSerialNo) {
	int iRet = GetVolumeInformationA(0, 0, 0, lpulVolumeSerialNo, 0, 0, 0, 0);
	if (iRet == 0)
	{

		return FALSE;
	}
	else {
		return TRUE;
	}
}