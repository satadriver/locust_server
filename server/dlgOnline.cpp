


#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgOnline.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "dlgCmd.h"
#include "command.h"
#include "dialog.h"
#include "FileHelper.h"

using namespace std;


DialogOnline* g_dlgOnline = 0;

DialogOnline::DialogOnline() {

}


DialogOnline::DialogOnline(string id) {
	m_id = id;
}

DialogOnline::~DialogOnline() {
	if (g_dlgOnline)
	{
		delete g_dlgOnline;
		g_dlgOnline = 0;
	}
}


vector<CLIENT_INFO> getOnlineInfo(const char * data,int size) {
	int offset = 0;

	vector<CLIENT_INFO> clients;

	while (offset < size)
	{
		int nl = *(data + offset);
		offset++;
		string id = string(data + offset, nl);
		offset += nl;

		int ipl = *(data + offset);
		offset++;
		string ip = string(data + offset, ipl);
		offset += ipl;

		int tl = *(data + offset);
		offset++;
		string t = string(data + offset, tl);
		offset += tl;

		CLIENT_INFO ci;
		ci.date = t;
		ci.host = id;
		ci.ip = ip;

		clients.push_back(ci);

		if (memcmp(data + offset, "@@@", 3) == 0)
		{
			offset += 3;
			break;
		}
	}
	return clients;
}


int __stdcall onlineInfo_old(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	string cmd = params->cmd;

	delete params;

	PacketParcel packet(TRUE, id);

	ret = packet.postCmd(CMD_READ_ONLINE, 0,0);

	char* data = packet.getbuf();
	size_t datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	string stronline = string(data + 4, datasize - 8);
	vector<CLIENT_INFO> clients = getOnlineInfo(data + 4, datasize - 8);
	if (clients.size())
	{
		HWND list = GetDlgItem(g_dlgOnline->m_hwnd, IDC_LIST3);
		for (int i = 0;i < clients.size();i ++)
		{
			char online[1024];

			wsprintfA(online, "ID:%s,IP:%s,TIME:%s", clients[i].host.c_str(), clients[i].ip.c_str(), clients[i].date.c_str());

			ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)online);
		}
	}
	
	return 0;
}



int __stdcall onlineInfo(CMD_PARAMS* params) {

	int ret = 0;
	int cnt = 0;

	string id = params->id;
	string cmd = params->cmd;

	delete params;
	string filename = id + "." + LOCAL_OBJECTS_FILENAME;

	char* file = 0;
	int filesize = 0;
	ret = FileHelper::fileReader(filename.c_str(), &file, &filesize);
	if (ret )
	{
		HWND list = GetDlgItem(g_dlgOnline->m_hwnd, IDC_LIST3);

		string str = string(file, filesize);

		do
		{
			int pos = str.find("\r\n");
			if (pos != str.npos)
			{
				pos += 2;
				string s = str.substr(0, pos);
				str = str.substr(pos);
				ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)s.c_str());
				cnt++;
			}
		} while (TRUE);
	}

	return cnt;
}


INT_PTR DialogOnline::dlgOnlineProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG)
	{

	}
	else if ((msg & 0xffff) == WM_COMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
		if (wparam == IDOK)
		{

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

	}
	else if (msg == WM_RBUTTONDOWN)
	{

	}
	else if (msg == WM_LBUTTONDOWN)
	{

	}
	return 0;
}



int __stdcall DialogOnline::runDlgOnline(DialogOnline* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG4, 0, (DLGPROC)dlgOnlineProc, 0);
	if (dialog->m_hwnd == 0)
	{
		ret = GetLastError();
		return FALSE;
	}

	ret = ShowWindow(dialog->m_hwnd, SW_SHOW);

	CMD_PARAMS *params = new CMD_PARAMS;
	params->id = dialog->m_id;
	params->cmd = "";
	HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)onlineInfo, params, 0, 0);
	if (ht )
	{
		CloseHandle(ht);
	}

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
	return 0;
}


int createDlgOnline(HINSTANCE hinst, string id) {
	g_dlgOnline = new DialogOnline(id);
	g_dlgOnline->m_hinst = hinst;
	//g_dlgCmd->runDlgCmd(g_dlgCmd);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgOnline->runDlgOnline, g_dlgOnline, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}